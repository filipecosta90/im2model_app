@echo off
echo.
echo ---------------------------------------------------
echo Creating a cif file from cel file %1.cel
echo ---------------------------------------------------
call cellmuncher -f %1.cel -o %1.cif -w CIF --override
echo ---------------------------------------------------
echo.