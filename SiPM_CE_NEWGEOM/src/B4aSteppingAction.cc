
/// \file B4aSteppingAction.cc
/// \brief Implementation of the B4aSteppingAction class

#include "B4aSteppingAction.hh"
#include "B4aEventAction.hh"
#include "B4DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "math.h"
extern G4double bins;// ****
extern G4double bins1;// ****
extern G4double T1;// ****
extern G4double Event_number ;// ****
G4double PMT_QE = 0.26; // average PMT quantum efficiency
G4Random * rnd = new G4Random();

B4aSteppingAction::B4aSteppingAction(
        const B4DetectorConstruction* detectorConstruction,
        B4aEventAction* eventAction)
    : G4UserSteppingAction(),
      fDetConstruction(detectorConstruction),
      fEventAction(eventAction)
{}

B4aSteppingAction::~B4aSteppingAction()
{}


void B4aSteppingAction::UserSteppingAction(const G4Step* step)
{

    auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
    const G4Track* track = step->GetTrack();
    G4String VolName = volume->GetName();
    G4String particlename = step->GetTrack()->GetParticleDefinition()->GetParticleName();
    auto posz=step->GetPreStepPoint()->GetPosition().z();
    auto posy=step->GetPreStepPoint()->GetPosition().y();
    auto posx=step->GetPreStepPoint()->GetPosition().x();
    auto f_posz=step->GetPostStepPoint()->GetPosition().z();
    auto f_posy=step->GetPostStepPoint()->GetPosition().y();
    auto f_posx=step->GetPostStepPoint()->GetPosition().x();

    auto Angle = step->GetTrack()->GetMomentumDirection().getTheta()/deg;
    /*
    if (particlename == "opticalphoton" ){
    std::cout
              << "Event "          << fEventAction->eventID << ", "
              << "Track "           << track->GetTrackID()
              << "("                << particlename                                                                 << ")"
              << ", step "          << track->GetCurrentStepNumber()
              << ", position: "     << std::setprecision(2)
              << "(" << posx/cm << ","<< posy/cm << ","<< posz/cm     << ")"
              << "-> "
              << "(" << f_posx/cm << ","<< f_posy/cm << ","<< f_posz/cm     << ") cm, "
              << std::endl
              << "volume-i "          << step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName()
              << "-> "
              << "volume-f "          << step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName()    << ", "
              << std::endl
              << "time: "         << step->GetTrack()->GetGlobalTime()/ns                                         << " ns "
              << std::endl;
}

*/
    //  G4StepPoint*thePostPoint=step->GetPostStepPoint();


    if ((particlename == "opticalphoton" ) && ( track->GetCurrentStepNumber() == 1))
    {
        T1 = step->GetTrack()->GetGlobalTime()/ns;
    }
    G4double Scint_dX = 2. * cm;
    G4double Scint_dY = 2. * cm;
    G4double Scint_dZ = 20.*cm; //Scintilator Length Det construction
    if (particlename == "opticalphoton" )
    {
        if( fabs(posz - (-Scint_dZ/2))<0.01*cm && (fabs(posx) < Scint_dX/2) && (fabs(posy) < Scint_dY/2))
        {
            fEventAction->PhotonHitUpperFace[track->GetTrackID()] = TRUE;
        }
        // (1) check if the optical photon arrived at the SiPM area

        if (VolName=="mySiPM")
        {

            // (2) if the photon arrived at the SiPM area, we want to determine if it was absorbed / detected
            // or not
            // We do this according to the PDE (PMT QE)
            double rnd_nbr = rnd->flat();
            if (rnd_nbr < PMT_QE){


                //    if (thePostPoint->GetProcessDefinedStep()->GetProcessName()=="OpAbsorption")
                //    {
                if (bins==0) fEventAction->FirstPhotonTime_mySiPM1 = step->GetTrack()->GetGlobalTime();

                bins=bins+1;// ****
                fEventAction->Nphotons_mySiPM1++;

               // std::ofstream myfile2;
               // myfile2.open("Time_PMT_Down.csv",std::ofstream::out|std::ofstream::app);
               // myfile2 << track->GetTrackID()
               //        << ","
               //         << (step->GetTrack()->GetGlobalTime()/ns)//-T1
               //         << ","
               //         << bins
               //         << ","
               //         << Event_number
               //         <<std::endl;
               // myfile2.close();
            }
            ((G4Track*)track)->SetTrackStatus(fStopAndKill);// ****
            //   }
        }
    }

    if (particlename == "gamma" && VolName == "World" )
    {
        ((G4Track*)track)->SetTrackStatus(fStopAndKill);// ****
    }
}























































































































































