#! usr/bin/python3
# -*- coding: utf-8 -*-

from flask import Flask, render_template, request
from werkzeug import secure_filename
import urllib.request
import urllib.error
import urllib.parse
import os
import json
#from urllib.parse import urlparse

app = Flask(__name__)
app.debug = True

#app.add_url_rule('/graphql', view_func=GraphQLView.as_view('graphql', schema=schema, graphiql=True, context={'session': db_session}))

@app.route('/upload')
def upload():
    return render_template('upload_cif.html')


@app.route('/api/cif/fetch', methods = ['POST'])
def fetch_file():
	data_dict = json.loads( request.data )
	print( data_dict )
	url_param = data_dict["url"]
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
	except ValueError as e:
		print( e )

	return 'file uploaded successfully'

@app.route('/api/cif/upload', methods = ['GET', 'POST'])
def upload_file():
    if request.method == 'POST':
        f = request.files['file']
        f.save(secure_filename(f.filename))
        return 'file uploaded successfully'

if __name__ == "__main__":
    app.run()
