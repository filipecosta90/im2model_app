#! usr/bin/python3
# -*- coding: utf-8 -*-

from flask import Flask, render_template, request
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

global apiVersion 
apiVersion = "0.0.1.1"

app = Flask(__name__)
app.config['JSONIFY_PRETTYPRINT_REGULAR'] = False

app.debug = True

@app.route('/upload')
def upload():
    return render_template('upload_cif.html')


@app.route('/api/cif/fetch', methods = ['POST'])
def fetch_file():
	data_dict = json.loads( request.data )
	print( data_dict )
	url_param = data_dict["url"]
	status = None
	data = {}
	try:
		base_url = urllib.parse.urlparse( url_param )
		base = os.path.basename( base_url.path )
		base_filename = os.path.splitext(base)[0]

		base_extension = os.path.splitext(base)[1]
		base_filename_with_ext = base_filename + base_extension
		if base_extension == ".html":
			base_url = base_url._replace( path=str( os.path.splitext(base_url.path)[0] + ".cif" ) )
			base_filename_with_ext = base_filename + ".cif"

		print( base_url.geturl() )
		cif_file = urllib.request.urlopen( base_url.geturl() )
		print("fetching {0} ( format {1} ) from {2}".format( base_filename, base_extension, url_param ) )
		with open( base_filename_with_ext ,'wb') as output:
			output.write( cif_file.read() )

		cell = ase.io.read( base_filename_with_ext ) 
		os.remove( base_filename_with_ext )
		data = prepare_cif_json( cell )
		status = True

	except ValueError as e:
		print( e )

	return_code = 404
	result = None
	if status is None:
		result = {
		        "apiVersion": apiVersion,
		        "params": request.args,
		        "method": "POST",
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
		        "method": "POST",
		        "took": 0,
		        "data" : data, 
		        }
		return_code = 200

	return jsonpify(result), return_code


@app.route('/api/cif/upload', methods = ['POST'])
def upload_file():
	status = None
	result = None
	print(request.__dict__)

	if request.method == 'POST':
		f = request.files['file']
		f.save(secure_filename(f.filename))
		cell = ase.io.read( f.filename )
		os.remove( f.filename )
		data = prepare_cif_json( cell )
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
		        }
		return_code = 200

	return jsonpify(result), return_code


if __name__ == "__main__":
    app.run( host='0.0.0.0', threaded=True )
