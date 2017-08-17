#ifndef __ATOM_INFO_H__
#define __ATOM_INFO_H__

#include <iosfwd>                 // for string
#include <opencv2/core/matx.hpp>  // for Vec4d
#include <string>                 // for basic_string

//! Encapsulates information regarding an atom type.
class Atom_Info
{
  public:
    //! Constructor.
    Atom_Info(
        std::string type_name,
        std::string type_symbol,
        int atomic_num,
        double atomic_mass,
        double melting_pt,
        double boiling_pt,
        double electroneg,
        double electron_aff,
        int valence,
        // ??
        double calculated_r,
        // empirical Radius, Unit: pm
        double empirical_r,
        // Covalent Radius, Unit: pm
        double covalent_r,
        //van der Waal's Radius, Unit: pm
        double vdw_r,
        unsigned int cpk_color,
        unsigned int rasmol_color
        );

    //! Returns the name of the atom.
    std::string name();

    //! Returns the symbol of the atom
    std::string symbol();

    //! Returns the atom's atomic number
    int atomicNumber();

    //! Returns the atom mass
    double atomicMass();

    //! Returns the element's melting point in Kelvin
    double meltingPoint();

    //! Returns the element's boiling point in Kelvin
    double boilingPoint();

    //! Returns the atom electronegativity
    double electronegativity();

    //! Returns the electron affinity in Kj/mol
    double electronAffinity();

    //! Returns the valence of the atom
    int valence();

    //! Returns the atom's calculated radius in Angstroms
    double calculatedRadius();

    //! Returns the atom's empirical radius in Angstroms
    double empiricalRadius_Angstroms();
    double empiricalRadius_Nanometers();

    //! Returns the atom's covalent radius in Angstroms
    double covalentRadius();

    //! Returns the atom's van der Waals radius in Angstroms
    double vanDerWaalsRadius_Angstroms();
    double vanDerWaalsRadius_Nanometers();

    //! Returns the atom's CPK color
    cv::Vec4d cpkColor();

    //! Returns the atom's RasMol color
    cv::Vec4d rasmolColor();

  private:

    /** CIF compliant with field atom_type_symbol
      The code used to identify the atom species (singular or plural) representing this atom type. Normally this code is the element symbol. The code may be composed of any character except an underscore with the additional proviso that digits designate an oxidation state and must be followed by a + or - character.
     **/
    std::string atom_type_symbol;
    std::string atom_type_name;
    int mAtomicNumber;
    int mValence;
    unsigned int mCPKColor;
    unsigned int mRasMolColor;
    double mAtomicMass;
    double mMeltingPoint;
    double mBoilingPoint;
    double mElectronegativity;
    double mElectronAffinity;
    double mCalculatedRadius;
    double mEmpiricalRadius_Picometers;
    double mCovalentRadius;
    double mVanDerWaalsRadius;
};

#endif
