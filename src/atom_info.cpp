
#include "atom_info.hpp"

//! Constructor.

Atom_Info::Atom_Info(
    std::string type_name,
    std::string type_symbol,
    int atomic_num,
    double atomic_mass,
    double melting_pt,
    double boiling_pt,
    double electroneg,
    double electron_aff,
    int valence,
    double calculated_r,
    double empirical_r,
    double covalent_r,
    double vdw_r,
    unsigned int cpk_color,
    unsigned int rasmol_color
    ){
  atom_type_name = type_name;
  atom_type_symbol = type_symbol;
  mAtomicNumber      = atomic_num;
  mAtomicMass        = atomic_mass;
  mMeltingPoint      = melting_pt;
  mBoilingPoint      = boiling_pt;
  mElectronegativity = electroneg;
  mElectronAffinity  = electron_aff;
  mValence           = valence;
  mCalculatedRadius  = calculated_r;
  mEmpiricalRadius_Picometers = empirical_r;
  mCovalentRadius    = covalent_r;
  mVanDerWaalsRadius = vdw_r;
  mCPKColor = cpk_color;
  mRasMolColor = rasmol_color;
}

//! Returns the name of the atom.
std::string Atom_Info::name() {
  return atom_type_name;
}

//! Returns the symbol of the atom
std::string Atom_Info::symbol() {
  return atom_type_symbol;
}

//! Returns the atom's atomic number
int Atom_Info::atomicNumber()  {
  return mAtomicNumber;
}

//! Returns the atom mass
double Atom_Info::atomicMass()  {
  return mAtomicMass;
}

//! Returns the element's melting point in Kelvin
double Atom_Info::meltingPoint()  {
  return mMeltingPoint;
}

//! Returns the element's boiling point in Kelvin
double Atom_Info::boilingPoint()  {
  return mBoilingPoint;
}

//! Returns the atom electronegativity
double Atom_Info::electronegativity()  {
  return mElectronegativity;
}

//! Returns the electron affinity in Kj/mol
double Atom_Info::electronAffinity()  {
  return mElectronAffinity;
}

//! Returns the valence of the atom
int Atom_Info::valence() {
  return mValence;
}

//! Returns the atom's calculated radius in Angstroms
double Atom_Info::calculatedRadius()  {
  return mCalculatedRadius / 100.0;
}

//! Returns the atom's empirical radius in Angstroms
double Atom_Info::empiricalRadius_Angstroms()  {
  return mEmpiricalRadius_Picometers / 100.0;
}

//! Returns the atom's empirical radius in Nanometers
double Atom_Info::empiricalRadius_Nanometers()  {
  return mEmpiricalRadius_Picometers / 1000.0;
}

//! Returns the atom's covalent radius in Angstroms
double Atom_Info::covalentRadius()  {
  return mCovalentRadius / 100.0;
}

//! Returns the atom's van der Waals radius in Angstroms
double Atom_Info::vanDerWaalsRadius_Angstroms()  {
  return mVanDerWaalsRadius / 100.0;
}

//! Returns the atom's van der Waals radius in Angstroms
double Atom_Info::vanDerWaalsRadius_Nanometers()  {
  return mVanDerWaalsRadius / 1000.0;
}

//! Returns the atom's CPK color
cv::Vec4d Atom_Info::cpkColor() {
  float r,g,b,a;
  r = ((mCPKColor >> 16) & 0xFF) / 255.0f;
  g = ((mCPKColor >> 8)  & 0xFF) / 255.0f;
  b =  (mCPKColor        & 0xFF) / 255.0f;
  a = 1.0f;
  cv::Vec4d Color(r,g,b,a);
  return Color;
}

//! Returns the atom's RasMol color
cv::Vec4d Atom_Info::rasmolColor() {
  float r,g,b,a;
  r = ((mRasMolColor >> 16) & 0xFF) / 255.0f;
  g = ((mRasMolColor >> 8)  & 0xFF) / 255.0f;
  b =  (mRasMolColor        & 0xFF) / 255.0f;
  a = 1.0f;
  cv::Vec4d Color(r,g,b,a);
  return Color;
}
