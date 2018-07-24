import sys;
from ase.io.cif import parse_cif; 
cif = parse_cif( 'CeO2.cif' )
for key,value in cif[0][1].items():
    print( "{0} -> {1} {2}".format( type( value ), key, value ) )
