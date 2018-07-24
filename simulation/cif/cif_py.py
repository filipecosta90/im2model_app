from CifFile import ReadCif

cf = ReadCif('4_SnO2_0.0288nm.cif')
for key,value in cf.items():
    #print( '{0}{1}'.format(key,value) )
    print( type(value))
