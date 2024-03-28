/// \file B4aEventAction.hh
/// \brief Definition of the B4aEventAction class

#ifndef B4aEventAction_h
#define B4aEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#define NphotonsMax 100000

/// Event action class
///
/// It defines data members to hold the energy deposit and track lengths
/// of charged particles in Absober and Gap layers:
/// - fEnergyAbs, fEnergyGap, fTrackLAbs, fTrackLGap
/// which are collected step by step via the functions
/// - AddAbs(), AddGap()

class B4aEventAction : public G4UserEventAction
{
  public:
    B4aEventAction();
    virtual ~B4aEventAction();

    virtual void  BeginOfEventAction(const G4Event* event);
    virtual void    EndOfEventAction(const G4Event* event);

    G4bool PhotonHitUpperFace[NphotonsMax];

    G4int eventID;

    // detection time in [ns] - global time of first photon detected
    G4double FirstPhotonTime_mySiPM1, FirstPhotonTime_mySiPM2;
    // number of photons that hit the "SiPM"s
    G4int    Nphotons_mySiPM1,        Nphotons_mySiPM2;

  private:

};



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif



























