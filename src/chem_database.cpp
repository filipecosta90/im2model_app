#include "chem_database.hpp"
#include <cctype>         // for isalpha
#include <iostream>       // for string, operator<<, basic_ostream, cout, endl
#include <map>            // for map, map<>::iterator, __map_iterator, opera...
#include <string>         // for operator<, allocator, basic_string<>::iterator
#include <utility>        // for pair
#include "atom_info.hpp"  // for Atom_Info

Chem_Database::Chem_Database(){
  // type_name, type_symbol, atomic_num, atomic_mass, melting_pt, boiling_pt, electroneg, electron_aff, valence, calculated_r, empirical_r, covalent_r, vdw_r, cpk_color, rasmol_color
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("H", Atom_Info::Atom_Info( "Hydrogen",    "H",   1, 1.00794,      14.01, 20.28,2.20,   72.8,   1, 53,  25,  37,  120, 0xFFFFFF, 0xFFFFFF ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("He", Atom_Info::Atom_Info( "Helium",      "He",  2, 4.002602,     0.95,   4.22,  -1,      0,   0, 31, -1,   32,  140, 0xD9FFFF, 0xFFC0CB ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("Li", Atom_Info::Atom_Info( "Lithium",     "Li",  3, 6.941,        453.69, 1615, 0.98,  59.6,   1, 167, 145, 134, 182, 0xCC80FF, 0xB22121 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("Be", Atom_Info::Atom_Info( "Beryllium",   "Be",  4, 9.012182,     1560,   2743, 1.57,     0,   2, 112, 105, 90, -1  , 0xC2FF00, 0xFF1493 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("B", Atom_Info::Atom_Info( "Boron",       "B",   5, 10.811,       2348,   4273, 2.04,  26.7,   3, 87,  85,  82, -1  , 0xFFB5B5, 0x00FF00 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("C", Atom_Info::Atom_Info( "Carbon",      "C",   6, 12.0107,      3823,   4300, 2.55, 153.9,   4, 67,  70,  77,  170, 0x909090, 0xD3D3D3 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("N", Atom_Info::Atom_Info( "Nitrogen",    "N",   7, 14.0067,      63.05, 77.36, 3.04,   7.0,   3, 56,  65,  75,  155, 0x3050F8, 0x87CEE6 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("O", Atom_Info::Atom_Info( "Oxygen",      "O",   8, 15.9994,      54.8,   90.2, 3.44,   141,   2, 48,  60,  73,  152, 0xFF0D0D, 0xFF0000 ) ) );
elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("F", Atom_Info::Atom_Info( "Fluorine",    "F",   9, 18.9984032,   53.5,  85.03, 3.98,   328,   1, 42,  50,  71,  147, 0x90E050, 0xDAA520 ) ) );
elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("Ne", Atom_Info::Atom_Info( "Neon",        "Ne",  10, 20.1797,     24.56, 27.07,    -1,    0,   0, 38, -1,   69,  154, 0xB3E3F5, 0xFF1493 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("Na", Atom_Info::Atom_Info( "Sodium",      "Na",  11, 22.98976928, 370.87, 1156,  0.93,52.08,   1, 190, 180, 154, 227, 0xAB5CF2, 0x0000FF ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("Mg", Atom_Info::Atom_Info( "Magnesium",   "Mg",  12, 24.3050,     923,    1363,  1.31,    0,   2, 145, 150, 130, 173, 0x8AFF00, 0x228B22 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("Al", Atom_Info::Atom_Info( "Aluminium",   "Al",  13, 26.9815386,  933,    2792,  1.61, 42.5,   3, 118, 125, 118,   0, 0xBFA6A6, 0x696969 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info > ("Si", Atom_Info::Atom_Info( "Silicon",     "Si",  14, 28.0855,     1687,   3173,  1.90,133.6,   4, 111, 110, 111, 210, 0xF0C8A0, 0xDAA520 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("P", Atom_Info::Atom_Info( "Phosphorus",  "P",   15, 30.973762,   317.3,  553.6, 2.19,   72,   5,  98, 100, 106, 180, 0xFF8000, 0xFFAA00 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("S", Atom_Info::Atom_Info( "Sulfur",      "S",   16, 32.065,      388.36, 717.87,2.58,  200,   6,  88, 100, 102, 180, 0xFFFF30, 0xFFFF00 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Cl", Atom_Info::Atom_Info( "Chlorine",    "Cl",  17, 35.453,      171.6,  239.11,3.16,  359,   5,  79, 100,  99, 175, 0x1FF01F, 0x00FF00 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ar", Atom_Info::Atom_Info( "Argon",       "Ar",  18, 39.948,      83.8,   87.3,    -1,    0,   0,  71,  71,  97, 188, 0x80D1E3, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("K", Atom_Info::Atom_Info( "Potassium",   "K",   19, 39.0983,     336.53, 1032,  0.82, 48.4,   1, 243, 220, 196, 275, 0x8F40D4, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ca", Atom_Info::Atom_Info( "Calcium",     "Ca",  20, 40.078,      1115,   1757,   1.0, 2.37,   2, 194, 180, 174,  -1, 0x3DFF00, 0x696969 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Sc", Atom_Info::Atom_Info( "Scandium",    "Sc",  21, 44.955912,   1814,   3103,   1.36, 18.1,  3, 184, 160, 144,  -1, 0xE6E6E6, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ti", Atom_Info::Atom_Info( "Titanium",    "Ti",  22, 47.867,      1941,   3560,   1.54, 7.6,   4, 176, 140, 136,  -1, 0xBFC2C7, 0x696969 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("V", Atom_Info::Atom_Info( "Vanadium",    "V",   23, 50.9415,     2183,   3680,   1.63, 50.6,  5, 171, 135, 125,  -1, 0xA6A6AB, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Cr", Atom_Info::Atom_Info( "Chromium",    "Cr",  24, 51.9961,     2180,   2944,   1.66, 64.3,  6, 166, 140, 127,  -1, 0x8A99C7, 0x696969 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Mn", Atom_Info::Atom_Info( "Manganese",   "Mn",  25, 54.938045,   1519,   2334,   1.55, 0,     4, 161, 140, 139,  -1, 0x9C7AC7, 0x696969 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Fe", Atom_Info::Atom_Info( "Iron",        "Fe",  26, 55.845,      1811,   3134,   1.83, 15.7,  3, 156, 140, 125,  -1, 0xE06633, 0xFFAA00 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Co", Atom_Info::Atom_Info( "Cobalt",      "Co",  27, 58.933195,   1768,   3200,   1.88, 63.7,  4, 152, 135, 126,  -1, 0xF090A0, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ni", Atom_Info::Atom_Info( "Nickel",      "Ni",  28, 58.6934,     1728,   3186,   1.91, 112,   4, 149, 135, 121, 163, 0x50D050, 0x802828 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Cu", Atom_Info::Atom_Info( "Copper",      "Cu",  29, 63.546,      1357.77,3200,   1.90, 118.4, 2, 145, 135, 138, 140, 0xC88033, 0x802828 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Zn", Atom_Info::Atom_Info( "Zinc",        "Zn",  30, 65.409,      692.68, 1180,   1.65, 0,     2, 142, 135, 131, 139, 0x7D80B0, 0x802828 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ga", Atom_Info::Atom_Info( "Gallium",     "Ga",  31, 69.723,      302.91, 2477,   1.81, 28.9,  3, 136, 130, 126, 187, 0xC28F8F, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ge", Atom_Info::Atom_Info( "Germanium",   "Ge",  32, 72.64,       1211.4, 3093,   2.01, 119,   4, 125, 125, 122,  -1, 0x668F8F, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("As", Atom_Info::Atom_Info( "Arsenic",     "As",  33, 74.92160,    1090,   887,    2.18, 78,    5, 114, 115, 119, 185, 0xBD80E3, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Se", Atom_Info::Atom_Info( "Selenium",    "Se",  34, 78.96,       494,    958,    2.55, 195,   6, 103, 115, 116, 190, 0xFFA100, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Br", Atom_Info::Atom_Info( "Bromine",     "Br",  35, 79.904,      265.8,  332,    2.96, 324.6, 7, 94,  115, 114, 185, 0xA62929, 0x802828 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Kr", Atom_Info::Atom_Info( "Krypton",     "Kr",  36, 83.798,      115.79, 119.93, 3.0,  0,     4, 88, -1,   110, 202, 0x5CB8D1, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Rb", Atom_Info::Atom_Info( "Rubidium",    "Rb",  37, 85.4678,     312.46, 961,    0.82, 46.9,  1, 265, 235, 211,  -1, 0x702EB0, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Sr", Atom_Info::Atom_Info( "Strontium",   "Sr",  38, 87.62,       1050,   1655,   0.95, 5.03,  2, 212, 200, 192,  -1, 0x00FF00, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Y", Atom_Info::Atom_Info( "Yttrium",     "Y",   39, 88.90585,    1799,   3618,   1.22, 29.6,  3, 212, 180, 162,  -1, 0x94FFFF, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Zr", Atom_Info::Atom_Info( "Zirconium",   "Zr",  40, 91.224,      2128,   4682,   1.33, 41.1,  4, 206, 155, 148,  -1, 0x94E0E0, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Nb", Atom_Info::Atom_Info( "Niobium",     "Nb",  41, 92.906,      2750,   5017,   1.6,  86.1,  5, 198, 145, 137,  -1, 0x73C2C9, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Mo", Atom_Info::Atom_Info( "Molybdenum",  "Mo",  42, 95.94,       2896,   4912,   2.16, 71.9,  6, 190, 145, 145,  -1, 0x54B5B5, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Tc", Atom_Info::Atom_Info( "Technetium",  "Tc",  43, 98.0,        2430,   4538,   1.9,  53,    7, 183, 135, 156,  -1, 0x3B9E9E, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ru", Atom_Info::Atom_Info( "Ruthenium",   "Ru",  44, 101.07,      2607,   4423,   2.2, 101.3,  6, 178, 130, 126,  -1, 0x248F8F, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Rh", Atom_Info::Atom_Info( "Rhodium",     "Rh",  45, 102.905,     2237,   3968,   2.28, 109.7, 6, 173, 135, 135,  -1, 0x0A7D8C, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Pd", Atom_Info::Atom_Info( "Palladium",   "Pd",  46, 106.42,      1828.05,3236,   2.20, 53.7,  4, 169, 140, 131, 163, 0x006985, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ag", Atom_Info::Atom_Info( "Silver",      "Ag",  47, 107.8682,    1234.93, 2435,  1.93, 125.6, 4, 165, 160, 153, 172, 0xC0C0C0, 0x696969 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Cd", Atom_Info::Atom_Info( "Cadmium",     "Cd",  48, 112.411,     594.22,  1040,  1.69, 0,     2, 161, 155, 148, 158, 0xFFD98F, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("In", Atom_Info::Atom_Info( "Indium",      "In",  49, 114.818,     429.75,  2345,  1.78, 28.9,  3, 156, 155, 144, 193, 0xA67573, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Sn", Atom_Info::Atom_Info( "Tin",         "Sn",  50, 118.710,     505.08,  2875,  1.96, 107.3, 4, 145, 145, 141, 217, 0x668080, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Sb", Atom_Info::Atom_Info( "Antimony",    "Sb",  51, 121.760,     903.78,  1860,  2.05, 103.2, 5, 133, 145, 138,  -1, 0x9E63B5, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Te", Atom_Info::Atom_Info( "Tellurium",   "Te",  52, 127.60,      722.66,  1261,  2.1,  190.2, 6, 123, 140, 135, 206, 0xD47A00, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("I", Atom_Info::Atom_Info( "Iodine",      "I",   53, 126.90447,   386.85,  457.4, 2.66, 295.2, 7, 115, 140, 133, 198, 0x940094, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Xe", Atom_Info::Atom_Info( "Xenon",       "Xe",  54, 131.293,     161.3,   165.1, 2.6,  0,     6, 108,-1,   130, 216, 0x429EB0, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Cs", Atom_Info::Atom_Info( "Caesium",     "Cs",  55, 132.9054519, 301.59,  944,   0.79, 45.5,  3, 298, 260, 225,  -1, 0x57178F, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ba", Atom_Info::Atom_Info( "Barium",      "Ba",  56, 137.327,     1000,    2143,  0.89, 13.95, 2, 253, 215, 198,  -1, 0x00C900, 0xFFAA00 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("La", Atom_Info::Atom_Info( "Lanthanum",   "La",  57, 138.90547,   1193,    3737,  1.10, 48,    3,-1,   195, 169,  -1, 0x70D4FF, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ce", Atom_Info::Atom_Info( "Cerium",      "Ce",  58, 140.116,     1071,    3633,  1.12, 50,    4,-1,   185,-1,    -1, 0xFFFFC7, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Pr", Atom_Info::Atom_Info( "Praseodymium", "Pr", 59, 140.90765,   1204,    3563,  1.13, 50,    4, 247, 185,-1,    -1, 0xD9FFC7, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Nd", Atom_Info::Atom_Info( "Neodymium",   "Nd",  60, 144.242,     1294,    3373,  1.14, 50,    3, 206, 185,-1,    -1, 0xC7FFC7, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Pm", Atom_Info::Atom_Info( "Promethium",  "Pm",  61, 145,         1373,    3273, -1,    50,    3, 205, 185,-1,    -1, 0xA3FFC7, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Sm", Atom_Info::Atom_Info( "Samarium",    "Sm",  62, 150.36,      1345,    2076,  1.17, 50,    3, 238, 185,-1,    -1, 0x8FFFC7, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Eu", Atom_Info::Atom_Info( "Europium",    "Eu",  63, 151.964,     1095,    1800, -1,    50,    3, 231, 185,-1,    -1, 0x61FFC7, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Gd", Atom_Info::Atom_Info( "Gadolinium",  "Gd",  64, 157.25,      1586,    3523,  1.20, 50,    3, 233, 180,-1,    -1, 0x45FFC7, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Tb", Atom_Info::Atom_Info( "Terbium",     "Tb",  65, 158.92535,   1629,    3503, -1,    50,    4, 225, 175,-1,    -1, 0x30FFC7, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Dy", Atom_Info::Atom_Info( "Dysprosium",  "Dy",  66, 162.500,     1685,    2840,  1.22, 50,    3, 228, 175,-1,    -1, 0x1FFFC7, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ho", Atom_Info::Atom_Info( "Holmium",     "Ho",  67, 164.93032,   1747,    2973,  1.23, 50,    3, 226, 175,-1,    -1, 0x00FF9C, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Er", Atom_Info::Atom_Info( "Erbium",      "Er",  68, 167.259,     1770,    3141,  1.24, 50,    3, 226, 175,-1,    -1, 0x00E675, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Tm", Atom_Info::Atom_Info( "Thulium",     "Tm",  69, 168.93421,   1818,    2223,  1.25, 50,    3, 222, 175,-1,    -1, 0x00D452, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Yb", Atom_Info::Atom_Info( "Ytterbium",   "Yb",  70, 173.04,      1092,    1469, -1,    50,    3, 222, 175,-1,    -1, 0x00BF38, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Lu", Atom_Info::Atom_Info( "Lutetium",    "Lu",  71, 174.967,     1936,    3675,  1.27, 50,    3, 217, 175, 160,  -1, 0x00AB24, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Hf", Atom_Info::Atom_Info( "Hafnium",     "Hf",  72, 178.49,      2506,    4876,  1.3,  0,     4, 208, 155, 150,  -1, 0x4DC2FF, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ta", Atom_Info::Atom_Info( "Tantalum",    "Ta",  73, 180.94788,   3290,    5731,  1.5,  31,    5, 220, 145, 138,  -1, 0x4DA6FF, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("W", Atom_Info::Atom_Info( "Tungsten",    "W",   74, 183.84,      3695,    5828,  2.36, 78.6,  6, 193, 135, 146,  -1, 0x2194D6, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Re", Atom_Info::Atom_Info( "Rhenium",     "Re",  75, 186.207,     3459,    5869,  1.9,  14.5,  7, 188, 135, 159,  -1, 0x267DAB, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Os", Atom_Info::Atom_Info( "Osmium",      "Os",  76, 190.23,      3306,    5285,  2.2,  106.1, 7, 185, 130, 128,  -1, 0x266696, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ir", Atom_Info::Atom_Info( "Iridium",     "Ir",  77, 192.217,     2739,    4701,  2.20, 151,   6, 180, 135, 137,  -1, 0x175487, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Pt", Atom_Info::Atom_Info( "Platinum",    "Pt",  78, 195.084,     2041,    4098,  2.28, 205.3, 6, 177, 135, 128, 175, 0xD0D0E0, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Au", Atom_Info::Atom_Info( "Gold",        "Au",  79, 196.966569,  1337.33, 3129,  2.54, 222.8, 7, 174, 135, 144, 166, 0xFFD123, 0xDAA520 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Hg", Atom_Info::Atom_Info( "Mercury",     "Hg",  80, 200.59,      234.32,  629.88,2.0 , 0,     2, 171, 150, 149, 155, 0xB8B8D0, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Tl", Atom_Info::Atom_Info( "Thallium",    "Tl",  81, 204.3833,    577,     1746,  1.62, 19.2,  3, 156, 190, 148, 196, 0xA6544D, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Pb", Atom_Info::Atom_Info( "Lead",        "Pb",  82, 207.2,       600.61,  2022,  2.33, 35.1,  4, 154, 180, 147, 202, 0x575961, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Bi", Atom_Info::Atom_Info( "Bismuth",     "Bi",  83, 208.98040,   544.4,   1837,  20.2, 91.2,  5, 143, 160, 146,  -1, 0x9E4FB5, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Po", Atom_Info::Atom_Info( "Polonium",    "Po",  84, 210,         527,     1235,  20.0, 183.3, 6, 135, 190,  -1,  -1, 0xAB5C00, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("At", Atom_Info::Atom_Info( "Astatine",    "At",  85, 210,         575,     610,   2.2,  270.1, 7, 127,  -1,  -1,  -1, 0x754F45, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Rn", Atom_Info::Atom_Info( "Radon",       "Rn",  86, 220,         202,     211.3,  -1,  0,     6, 120,  -1, 145,  -1, 0x428296, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Fr", Atom_Info::Atom_Info( "Francium",    "Fr",  87, 223,         300,     950,   0.7, -1,     3,  -1,  -1,  -1,  -1, 0x420066, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ra", Atom_Info::Atom_Info( "Radium",      "Ra",  88, 226,         973,     2010,  0.9, -1,     2,  -1, 215,  -1,  -1, 0x007D00, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ac", Atom_Info::Atom_Info( "Actinium",    "Ac",  89, 227,         1323,    3473,  1.1, -1,     3,  -1, 195,  -1,  -1, 0x70ABFA, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Th", Atom_Info::Atom_Info( "Thorium",     "Th",  90, 232.03806,   2023,    5093,  1.3, -1,     4,  -1, 180,  -1,  -1, 0x00BAFF, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Pa", Atom_Info::Atom_Info( "Protactinium", "Pa", 91, 231.03588,   1845,    4273,  1.5, -1,     5,  -1, 180,  -1,  -1, 0x00A1FF, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("U", Atom_Info::Atom_Info( "Uranium",     "U",   92, 238.02891,   1408,    4200,  1.38,-1,     6,  -1, 175,  -1, 186, 0x008FFF, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Np", Atom_Info::Atom_Info( "Neptunium",   "Np",  93, 237,         917,     4273,  1.36,-1,     6,  -1, 175,  -1,  -1, 0x0080FF, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Pu", Atom_Info::Atom_Info( "Plutonium",   "Pu",  94, 244,         913,     3503,  1.28,-1,     6,  -1, 175,  -1,  -1, 0x006BFF, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Am", Atom_Info::Atom_Info( "Americium",   "Am",  95, 243,         1449,    2284,  1.3, -1,     4,  -1, 175,  -1,  -1, 0x545CF2, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Cm", Atom_Info::Atom_Info( "Curium",      "Cm",  96, 247,         1618,    3383,  1.3, -1,     4,  -1,  -1,  -1,  -1, 0x785CE3, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Bk", Atom_Info::Atom_Info( "Berkelium",   "Bk",  97, 247,         1323,   -1,     1.3, -1,     4,  -1,  -1,  -1,  -1, 0x8A4FE3, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Cf", Atom_Info::Atom_Info( "Californium", "Cf",  98, 251,         1173,   -1,     1.3, -1,     4,  -1,  -1,  -1,  -1, 0xA136D4, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Es", Atom_Info::Atom_Info( "Einsteinium", "Es",  99, 252,         1133,   -1,     1.3, -1,     4,  -1,  -1,  -1,  -1, 0xB31FD4, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Fm", Atom_Info::Atom_Info( "Fermium",     "Fm",  100, 257,        1800,   -1,     1.3, -1,     3,  -1,  -1,  -1,  -1, 0xB31FBA, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Md", Atom_Info::Atom_Info( "Mendelevium", "Md",  101, 258,        1100,   -1,     1.3, -1,     3,  -1,  -1,  -1,  -1, 0xB30DA6, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("No", Atom_Info::Atom_Info( "Nobelium",    "No",  102, 259,        1100,   -1,     1.3, -1,     3,  -1,  -1,  -1,  -1, 0xBD0D87, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Lr", Atom_Info::Atom_Info( "Lawrencium",  "Lr",  103, 262,        1900,   -1,      -1, -1,     3,  -1,  -1,  -1,  -1, 0xC70066, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Rf", Atom_Info::Atom_Info( "Rutherfordium", "Rf",104, 261,          -1,   -1,      -1, -1,     4,  -1,  -1,  -1,  -1, 0xCC0059, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Db", Atom_Info::Atom_Info( "Dubnium",     "Db",  105, 262,          -1,   -1,      -1, -1,     5,  -1,  -1,  -1,  -1, 0xD1004F, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Sg", Atom_Info::Atom_Info( "Seaborgium",  "Sg",  106, 266,          -1,   -1,      -1, -1,     6,  -1,  -1,  -1,  -1, 0xD90045, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Bh", Atom_Info::Atom_Info( "Bohrium",     "Bh",  107, 264,          -1,   -1,      -1, -1,     7,  -1,  -1,  -1,  -1, 0xE00038, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Hs", Atom_Info::Atom_Info( "Hassium",     "Hs",  108, 277,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xE6002E, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Mt", Atom_Info::Atom_Info( "Meitnerium",  "Mt",  109, 268,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xEB0026, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Ds", Atom_Info::Atom_Info( "Darmstadtium", "Ds", 110, 271,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xF00024, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Rg", Atom_Info::Atom_Info( "Roentgenium", "Rg",  111, 272,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xF00024, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Uub", Atom_Info::Atom_Info( "Ununbium",    "Uub", 112, 285,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xF00024, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Uut", Atom_Info::Atom_Info( "Ununtrium",   "Uut", 113, 284,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xF00024, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Uuq", Atom_Info::Atom_Info( "Ununquadium", "Uuq", 114, 289,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xF00024, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Uup", Atom_Info::Atom_Info( "Ununpentium", "Uup", 115, 288,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xF00024, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Uuh", Atom_Info::Atom_Info( "Ununhexium",  "Uuh", 116, 292,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xF00024, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Uus", Atom_Info::Atom_Info( "Ununseptium", "Uus", 117, 293,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xF00024, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("Uuo", Atom_Info::Atom_Info( "Ununoctium",  "Uuo", 118, 294,          -1,   -1,      -1, -1,     6,  -1,  -1,  -1,  -1, 0xF00024, 0xFA1691 ) ) );
  elements_database.insert( std::pair<std::string, Atom_Info::Atom_Info> ("-", Atom_Info::Atom_Info( "Unknown",       "-",  -1,  -1,          -1,   -1,      -1, -1,    -1,  -1,  -1,  -1,  -1, 0xFA1691, 0xFA1691 ) ) );
}

Atom_Info Chem_Database::get_atom_info( std::string type_symbol ){
  std::string type_symbol_alnum = type_symbol;


  for(std::string::iterator i = type_symbol_alnum.begin(); i != type_symbol_alnum.end(); i++)
  {
    if(!isalpha(type_symbol_alnum.at(i - type_symbol_alnum.begin())))
    {
      type_symbol_alnum.erase(i);
      i--;
    }
  }
// std::cout << "element type symbol " << type_symbol << " , alnum " << type_symbol_alnum << std::endl;
  std::map<std::string,Atom_Info::Atom_Info>::iterator Atom_itt; 
  Atom_itt = elements_database.find( type_symbol_alnum );
  if ( Atom_itt == elements_database.end()){
    std::cout << "element not found in element DB" << std::endl;
    Atom_itt = elements_database.find( "-" );
  }
  Atom_Info element = Atom_itt->second; 
  std::cout << element.name() << std::endl;
  return element;
}

int Chem_Database::size(){
  return elements_database.size();
}

