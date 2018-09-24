#! usr/bin/python3
# -*- coding: utf-8 -*-

from flask import Flask, render_template, request, redirect
from flask_jsonpify import jsonpify

from werkzeug import secure_filename
import urllib.request
import urllib.error
import urllib.parse
import os
import json
import sys
import ase.io.cif
from ase import Atoms
import zlib
import sqlite3
import ntpath


#### globals ####
global cellsDBPath
cellsDBPath = "data/cellsdb.sqlite3"

global apiVersion 
apiVersion = "0.0.1.1"

UPLOAD_FOLDER = './uploads'
#### globals ####

def create_or_open_cells_db(db_file):
    db_exists = os.path.exists(db_file)
    connection = sqlite3.connect(db_file)
    if db_exists is False:
        sql = '''create table if not exists unitcells(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        result BLOB );'''
        connection.execute(sql)
    return connection

#backgroud save_cells_unitcells_data job
def save_cells_unitcells_data( id, cell_json, cellsDBPath ):
	inserted_key = None
	compressed_result = zlib.compress( json.dumps(cell_json).encode('utf-8') )
	sqlite3_conn = create_or_open_cells_db(cellsDBPath)
	sql = '''insert into unitcells ( id, result ) VALUES( ?, ? );'''
	if id is None:
		id_key = None
	else:
		id_key =  id 
	sqlite3_conn.execute( sql,[ id_key, sqlite3.Binary(compressed_result) ] )
	sqlite3_conn.commit()
	cur = sqlite3_conn.cursor()
	sql_select = "SELECT last_insert_rowid();"
	cur.execute(sql_select)
	result_string = cur.fetchone()
	if result_string:
		inserted_key = result_string[0]

	sqlite3_conn.close()
	return inserted_key

#backgroud save_cells_unitcells_data job
def get_cells_unitcells_data( id, cellsDBPath ):
	result = None
	inserted_key = None
	sqlite3_conn = create_or_open_cells_db(cellsDBPath)
	cur = sqlite3_conn.cursor()
	sql = "SELECT result FROM unitcells WHERE id = {0}".format(id)
	cur.execute(sql)
	result_binary = cur.fetchone()
	if result_binary:
		decompressed_result = zlib.decompress(result_binary[0])
		result = json.loads(decompressed_result.decode('utf-8'))
		print( result )
	sqlite3_conn.close()
	return result
	
def prepare_cif_json( cell ):
	data = {}
	a, b, c, alpha, beta, gamma = cell.get_cell_lengths_and_angles()

	data["_cell_length_a"] = a
	data["_cell_length_b"] = b
	data["_cell_length_c"] = c
	data["_cell_angle_alpha"] = alpha
	data["_cell_angle_beta"] = beta
	data["_cell_angle_gamma"] = gamma

	data["_cell_volume"] = cell.get_volume()

	data["_atom_site_fract_x"] = []
	data["_atom_site_fract_y"] = []
	data["_atom_site_fract_z"] = []
	data["_atom_site_occupancy"] = []

	data["_symmetry_equiv_pos_as_xyz"] = []
	data["_symmetry_equiv_pos_as_xyz"].append( 'x, y, z' )

	scaled_positions = cell.get_scaled_positions()
	for atom_site in scaled_positions:
		data["_atom_site_fract_x"].append(atom_site[0])
		data["_atom_site_fract_y"].append(atom_site[1])
		data["_atom_site_fract_z"].append(atom_site[2])
		data["_atom_site_occupancy"].append(1.0)

	data["_chemical_symbols"] = cell.get_chemical_symbols()

	return data

app = Flask(__name__)
app.config['JSONIFY_PRETTYPRINT_REGULAR'] = False
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

app.debug = True

@app.route('/upload')
def upload():
    return render_template('upload_cif.html')


@app.route('/api/cells/unitcells/cif/fetch', methods = ['POST'])
def fetch_file():
	global apiVersion
	global cellsDBPath
	print( request.headers )
	data_dict = json.loads( request.data )
	url_param = data_dict["url"]
	unitcell_link = None
	status = None
	data = {}
	try:
		base_url = urllib.parse.urlparse( url_param )
		self_url = urllib.parse.urlparse( request.host_url )
		if base_url.netloc == self_url.netloc:
			return redirect(url_param, code=301)
			
		base = os.path.basename( base_url.path )
		base_filename = os.path.splitext(base)[0]
		base_extension = os.path.splitext(base)[1]
		base_filename_with_ext = base_filename + base_extension
		if base_extension == ".html":
			base_url = base_url._replace( path=str( os.path.splitext(base_url.path)[0] + ".cif" ) )
			base_filename_with_ext = base_filename + ".cif"

		cif_file = urllib.request.urlopen( base_url.geturl() )
		with open( base_filename_with_ext ,'wb') as output:
			output.write( cif_file.read() )

		cell = ase.io.read( base_filename_with_ext ) 
		os.remove( base_filename_with_ext )

		data = prepare_cif_json( cell )
		inserted_cell_id = save_cells_unitcells_data(None, data, cellsDBPath)
		unitcell_link = "{0}api/cells/unitcells/{1}".format( request.host_url, inserted_cell_id )
		status = True

	except ValueError as e:
		print( e )

	return_code = 404
	result = None
	if status is None:
		result = {
		        "apiVersion": apiVersion,
		        "params": request.args,
		        "method": request.method,
		        "took": 0,
		        "error" : {
		            "code": 404,
		            "message": "Something went wrong.",
		            "url": request.url,
		            }, 
		        }
		return_code = 404
	else:
		result = {
		        "apiVersion": apiVersion,
		        "params": request.args,
		        "method": request.method,
		        "took": 0,
		        "data" : data, 
		        "links" : { "cell" : { "unitcell" : unitcell_link } }, 
		        }
		return_code = 200

	return jsonpify(result), return_code

@app.route('/api/cells/unitcells/<string:cellid>', methods = ['GET','POST'])
def api_cells_unitcells_get(cellid):
	global apiVersion
	global cellsDBPath
	status = None
	result = None
	data = get_cells_unitcells_data(cellid,cellsDBPath)

	if data is None:
		result = {
		        "apiVersion": apiVersion,
		        "params": request.args,
		        "method": request.method,
		        "took": 0,
		        "error" : {
		            "code": 404,
		            "message": "Something went wrong.",
		            "url": request.url,
		            },
		        }
		return_code = 404
	else:
		unitcell_link = "{0}api/cells/unitcells/{1}".format( request.host_url, cellid )
		result = {
		        "apiVersion": apiVersion,
		        "params": request.args,
		        "method": request.method,
		        "took": 0,
		        "data" : data,
		        "links" : { "cell" : { "unitcell" : unitcell_link } }, 
		        }
		return_code = 200

	return jsonpify(result), return_code


@app.route('/api/cells/unitcells/cif/upload', methods = ['POST'])
def upload_file():
	global apiVersion
	global cellsDBPath
	status = None
	result = None
	unitcell_link = None
	print( request.headers )

	if request.method == 'POST':
		if 'file' in request.files:
			file = request.files['file']
			filename = secure_filename( ntpath.basename( file.filename ) )
			filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename ) 
			file.save( filepath )
			#secure version of filename
			cell = ase.io.read( filepath )
			os.remove( filepath )
			data = prepare_cif_json( cell )
			inserted_cell_id = save_cells_unitcells_data(None, data, cellsDBPath)
			unitcell_link = "{0}api/cells/unitcells/{1}".format( request.host_url, inserted_cell_id )
			status = True

	if status is None:
		result = {
		        "apiVersion": apiVersion,
		        "params": request.args,
		        "method": request.method,
		        "took": 0,
		        "error" : {
		            "code": 404,
		            "message": "Something went wrong.",
		            "url": request.url,
		            },
		        }
		return_code = 404
	else:
		result = {
		        "apiVersion": apiVersion,
		        "params": request.args,
		        "method": request.method,
		        "took": 0,
		        "data" : data, 
		        "links" : { "cell" : { "unitcell" : unitcell_link } }, 
		        }
		return_code = 200

	return jsonpify(result), return_code


if __name__ == "__main__":
    app.run( host='0.0.0.0', threaded=True )
