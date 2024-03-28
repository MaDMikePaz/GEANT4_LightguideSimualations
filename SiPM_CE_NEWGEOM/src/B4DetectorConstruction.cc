/// \file B4DetectorConstruction.cc
/// \brief Implementation of the B4DetectorConstruction class

#include "B4DetectorConstruction.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "math.h"

#define PI 3.14159265

#include "G4Paraboloid.hh"
G4ThreadLocal
G4GlobalMagFieldMessenger* B4DetectorConstruction::fMagFieldMessenger = nullptr;
B4DetectorConstruction::B4DetectorConstruction()
 : G4VUserDetectorConstruction(),
   fAbsorberPV(nullptr),
   fGapPV(nullptr),
   fCheckOverlaps(true)
{
}

B4DetectorConstruction::~B4DetectorConstruction()
{
}

G4VPhysicalVolume* B4DetectorConstruction::Construct()
{
  return DefineVolumes();
}


G4VPhysicalVolume* B4DetectorConstruction::DefineVolumes()
{
    // Lead material defined using NIST Manager
    auto nistManager = G4NistManager::Instance();
    nistManager->FindOrBuildMaterial("G4_Pb");

    // Liquid argon material
    G4double a;  // mass of a mole;
    G4double z;  // z=mean number of protons;
    G4double density;
    new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
    new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,kStateGas, 2.73*kelvin, 3.e-18*pascal);
    G4Element* H =  new G4Element("Hydrogen",  "H",   z=1.,    a=1.01*g/mole);
    G4Element* C =  new G4Element("Carbon",    "C",   z=6.,    a=12.01*g/mole);
    G4Element* N =  new G4Element("Nitrogen",    "N",   z=7.,    a=14.01*g/mole);
    G4Element* O =  new G4Element("Oxygen",    "O",   z=8.,    a=16.00*g/mole);
    G4Element* Si =  new G4Element("Silicon",    "Si",   z=14.,    a=28.09*g/mole);

   //Optical Grease
     G4Material* Grease = new G4Material("Grease", 1.2*g/cm3, 3);
     Grease->AddElement( C, 18);
     Grease->AddElement( H, 20);
     Grease->AddElement( O, 3);
     G4double OpG_Energy[2] = {1.0*eV,10.0*eV};
     G4double OpG_Rindex[2] = {1.58,1.58};
     G4double OpG_ABSL[2] = {50.*m,50.*m};
     G4MaterialPropertiesTable*OpG_MPT=new G4MaterialPropertiesTable();
     OpG_MPT->AddProperty("ABSLENGTH",OpG_Energy,OpG_ABSL,2);
     OpG_MPT->AddProperty("RINDEX",OpG_Energy,OpG_Rindex,2);
     Grease->SetMaterialPropertiesTable(OpG_MPT);

    //PMMA
     G4Material* PMMA = new G4Material("PMMA", 1.18*g/cm3, 3);
     PMMA->AddElement( C, 5);
     PMMA->AddElement( H, 8);
     PMMA->AddElement( O, 2);

    //Air - will be used a s the material for our vikuity
    G4Material* Air = new G4Material("Air", 1.290*mg/cm3, 2);
    Air->AddElement( N, 0.7);
    Air->AddElement( O, 0.3);
   // G4MaterialPropertiesTable* Air_MPT = new G4MaterialPropertiesTable();



    //PVT - Polyvinyltoluene - BC408 , EJ-200
      G4Material* PVT = new G4Material("PVT", 1.023*g/cm3, 2);
      PVT->AddElement( C, 9);
      PVT->AddElement( H, 10);
    //--------PVT - Polyvinyltoluene - BC408 , EJ-200 Optics Start --------------------------------------------------------------------------------------------------------------//
      const G4int NUMENTRIES = 18; //Number of entries

      //Wavelengthes for refractive Index and Emission Spectra for BC-408/EJ200 (1240/wavelength[nm] = ev)
      G4double Scnt_PP[NUMENTRIES]   =   { 2.479*eV,2.504*eV,2.530*eV,2.583*eV,2.6101*eV,2.637*eV,2.666*eV,2.695*eV
                                         ,2.724*eV,2.755*eV,2.786*eV,2.81*eV,2.863*eV,2.883*eV,2.917*eV,2.952*eV
                                         ,2.973*eV,3.023*eV};

      //PVT entries for Emission Spectra of EJ440 scintillator from Eljen
      G4double Scnt_FAST[NUMENTRIES] = {0.03,0.06,0.09,0.15,0.21,0.3,0.38,0.41,0.48,0.53,0.62,0.73,0.9,0.97,1,0.9,0.7,0.3};

      //Scintillator refractive Index is set to be like the Binders
       G4double rindex[NUMENTRIES]={
                        1.58, 1.58, 1.58, 1.58, 1.58,
                        1.58, 1.58, 1.58, 1.58, 1.58,
                        1.58, 1.58, 1.58, 1.58, 1.58,
                        1.58, 1.58, 1.58};

      //PVT entries for Optical Absorption of Eljen scintillator
        G4double absorption[NUMENTRIES] = {160*cm,160*cm,160*cm,160*cm,160*cm,160*cm,
                                            160*cm,160*cm,160*cm,160*cm,160*cm,160*cm,
                                             160*cm,160*cm,160*cm,160*cm,160*cm,160*cm};


       //Definition of our scintillator
        G4MaterialPropertiesTable* Scnt_MPT = new G4MaterialPropertiesTable();
        Scnt_MPT->AddConstProperty("SCINTILLATIONYIELD", 10000./MeV);
        Scnt_MPT->AddConstProperty("RESOLUTIONSCALE", 1);
        Scnt_MPT->AddProperty("ABSLENGTH",Scnt_PP,absorption,NUMENTRIES);
        Scnt_MPT->AddProperty("RINDEX",Scnt_PP,rindex,NUMENTRIES);
        Scnt_MPT->AddProperty("FASTCOMPONENT", Scnt_PP, Scnt_FAST, NUMENTRIES);
        Scnt_MPT->AddConstProperty("FASTTIMECONSTANT",  2.*ns);
        Scnt_MPT->AddConstProperty("YIELDRATIO", 1.0);

        //ADD BIRKS CONSTANT
        PVT->SetMaterialPropertiesTable(Scnt_MPT);
        PVT->GetIonisation()->SetBirksConstant(0.126*mm/MeV);


        //----------Optics end--------------------------------------------------------------------------------------------------------------//
        //SiPM-------------------------------------------------------------------------------------------------------
        G4double den_pmtwindow = 2.20*g/cm3;
        G4Material *QuartzWindow = new G4Material("QuartzWindow", den_pmtwindow, 2 );
        QuartzWindow->AddElement( O, 2 );
        QuartzWindow->AddElement( Si, 1 );
        const G4int nentries_quartz = 51;
        G4double Ephoton_quartz[nentries_quartz] = {
        1.77120301*eV,  1.796872619*eV, 1.823297216*eV, 1.850510608*eV, 1.878548647*eV,
        1.907449396*eV, 1.937253292*eV, 1.968003345*eV, 1.999745334*eV, 2.032528044*eV,
        2.066403512*eV, 2.1014273*eV,   2.137658805*eV, 2.175161591*eV, 2.214003763*eV,
        2.254258377*eV, 2.296003902*eV, 2.33932473*eV,  2.384311744*eV, 2.431062955*eV,
        2.479684214*eV, 2.530290015*eV, 2.58300439*eV,  2.63796193*eV,  2.695308928*eV,
        2.755204682*eV, 2.817822971*eV, 2.883353737*eV, 2.952005017*eV, 3.024005139*eV,
        3.099605268*eV, 3.179082326*eV, 3.262742387*eV, 3.350924614*eV, 3.444005853*eV,
        3.54240602*eV,  3.646594433*eV, 3.757097294*eV, 3.874506585*eV, 3.999490668*eV,
        4.132807024*eV, 4.275317611*eV, 4.428007525*eV, 4.592007804*eV, 4.768623489*eV,
        4.959368428*eV, 5.16600878*eV,  5.390617857*eV, 5.635645941*eV, 5.904010034*eV,
        6.199210536*eV
        };
        G4double Rindex_quartz[nentries_quartz] = {
        1.455292466, 1.455524071, 1.455763571, 1.456011496, 1.456268423, 1.456534974, 1.456811819, 1.457099689,
        1.457399374, 1.457711733, 1.458037702, 1.4583783,   1.458734641, 1.459107942, 1.459499536, 1.459910886,
        1.460343603, 1.460799458, 1.461280408, 1.461788618, 1.462326487, 1.462896682, 1.463502175, 1.464146283,
        1.464832722, 1.465565665, 1.466349815, 1.467190482, 1.46809369,  1.469066293, 1.470116119, 1.471252144,
        1.472484709, 1.473825777, 1.475289258, 1.476891413, 1.478651361, 1.48059172,  1.482739429, 1.485126813,
        1.487792976, 1.490785646, 1.494163661, 1.498000361, 1.502388312, 1.507446007, 1.513327606, 1.520237459,
        1.528452449, 1.53835762,  1.550505538
        };

        const G4int Nabs_quartz = 10;
        G4double Ephoton_abs_quartz[Nabs_quartz] = {1.7491*eV,1.8793*eV,2.0012*eV,2.1734*eV,2.2993*eV,2.4195*eV,4.0372*eV,4.8901*eV,6.0646*eV,6.4897*eV};
        G4double Abslength_quartz[Nabs_quartz] = {522.51*cm,2666.17*cm,1633.49*cm,756.50*cm,418.96*cm,233.58*cm,23.46*cm,22.58*cm,15.76*cm,14.74*cm};
        G4MaterialPropertiesTable* MPT_temp= new G4MaterialPropertiesTable();
        MPT_temp->AddProperty("RINDEX", Ephoton_quartz, Rindex_quartz, nentries_quartz);
        MPT_temp->AddProperty("ABSLENGTH", Ephoton_abs_quartz, Abslength_quartz, Nabs_quartz);
        QuartzWindow->SetMaterialPropertiesTable(MPT_temp);

        //PERSPEX -----------------------------------------------------------------------------------------------------
        G4Material* PERSPEX = new G4Material("PERSPEX",1.032*g/cm3,2);
        PERSPEX->AddElement(H,11);
        PERSPEX->AddElement(C,10);
        G4MaterialPropertiesTable* PERSPEX_Scint = new G4MaterialPropertiesTable();
        const G4int PERSPEX_NUM = 15;
        G4double PERSPEX_PP[PERSPEX_NUM]   = {370.0*nm,380.0*nm,387.0*nm,393.0*nm,395.0*nm,398.0*nm,402.0*nm,413.0*nm,418.0*nm,435.0*nm,450.0*nm,460.0*nm,480.0*nm,500.0*nm,510*nm };
        G4double PERSPEX_refr[PERSPEX_NUM] = {1.49 ,1.49 ,1.49 ,1.49 ,1.49 ,1.49 ,1.49 ,1.49 ,1.49 ,1.49 ,1.49 ,1.49 ,1.49 ,1.49 ,1.49};
        G4double PERSPEX_abs[PERSPEX_NUM]  = {200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm,200.0*cm};
        PERSPEX_Scint->AddProperty("RINDEX",PERSPEX_PP,PERSPEX_refr,PERSPEX_NUM);
        PERSPEX_Scint->AddProperty("ABSLENGTH",PERSPEX_PP,PERSPEX_abs,PERSPEX_NUM);
        PERSPEX_Scint->AddConstProperty("SCINTILLATIONYIELD",0./MeV);//11832
        PERSPEX_Scint->AddConstProperty("RESOLUTIONSCALE",0.0);
        PERSPEX->SetMaterialPropertiesTable(PERSPEX_Scint);
        //PERSPEX ------------------------------------------------------------------------------------------------------


 //----------Geometry--------------------------------------------------------------------------------------------------------------//

  // Geometry parameters
  auto worldSizeXY = 100.*m/2;
  auto worldSizeZ  = 100.*m/2;
  G4double X = 3.*cm/2;
  G4double Y = 3.*cm/2;
  G4double Z = 3.*cm/2; //Scintilator Length as Stepping action
  G4double L = 0.2*mm/2;
  //Grease-----------------------------------------------------------------------------------------------------------

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Air");

  // World
  auto worldS  = new G4Box("World",worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size
  auto worldLV= new G4LogicalVolume( worldS,defaultMaterial,"World");         // its name
  auto worldPV = new G4PVPlacement(0, G4ThreeVector(0 ,0 ,0),worldLV,"World",0,false,0,fCheckOverlaps);  // checking overlaps
  //Scintilator------------------------------------------------------------------------------------------------------------
  G4Box *mybox = new G4Box("mybox",X,Y,Z);
  G4LogicalVolume *box_log = new G4LogicalVolume(mybox,PVT,"box_log");
  G4PVPlacement *box_layer = new G4PVPlacement(0,G4ThreeVector(),box_log,"PVT_Scint",worldLV,0,false,0);

  //Grease ------------------------------------------------------------------------------------------------------------
  G4double ZGrease = 0.1*mm/2;
  G4Box *myGrease = new G4Box("myGrease",X,Y,ZGrease);
  G4LogicalVolume *myGrease_log = new G4LogicalVolume(myGrease,Grease,"box_log");
  G4PVPlacement *myGrease_layer = new G4PVPlacement(0,G4ThreeVector(0,0,Z+ZGrease),myGrease_log,"myGrease",worldLV,0,false,0);

  //Light guide ------------------------------------------------------------------------------------------------------------
  G4double X1 = 2.*cm/2;
  G4double Y1 = 6.*mm/2;
  G4double Z5 = ((tan( 5 * PI / 180.0 )*(X1))-(tan(5 * PI / 180.0 )*(Y1)));
  G4double Z10 = ((tan( 10 * PI / 180.0 )*(X1))-(tan(10 * PI / 180.0 )*(Y1)));
  G4double Z15 = ((tan( 15 * PI / 180.0 )*(X1))-(tan(15 * PI / 180.0 )*(Y1)));
  G4double Z25 = ((tan( 25 * PI / 180.0 )*(X1))-(tan(25 * PI / 180.0 )*(Y1)));
  G4double Z35 = ((tan( 35 * PI / 180.0 )*(X1))-(tan(45 * PI / 180.0 )*(Y1)));
  G4double Z45 = ((tan( 45 * PI / 180.0 )*(X1))-(tan(45 * PI / 180.0 )*(Y1)));
  G4double Z60 = ((tan( 60 * PI / 180.0 )*(X1))-(tan(60 * PI / 180.0 )*(Y1)));
  G4double Z75 = ((tan( 75 * PI / 180.0 )*(X1))-(tan(75 * PI / 180.0 )*(Y1)));
  G4double Z85 = ((tan( 85 * PI / 180.0 )*(X1))-(tan(85 * PI / 180.0 )*(Y1)));
  std::cout << "Z5: " << Z5 << std::endl;
  std::cout << "Z10: " << Z10 << std::endl;
  std::cout << "Z15: " << Z15 << std::endl;
  std::cout << "Z25: " << Z25 << std::endl;
  std::cout << "Z35: " << Z35 << std::endl;
  std::cout << "Z45: " << Z45 << std::endl;
  std::cout << "Z60: " << Z60 << std::endl;
  std::cout << "Z75: " << Z75 << std::endl;
  std::cout << "Z85: " << Z85 << std::endl;

  G4double ZLG=Z75;
  G4Trd *LG = new G4Trd("myLG",X1,Y1,X1,Y1,ZLG);
  G4LogicalVolume *LG_log = new G4LogicalVolume(LG,PERSPEX,"LG_log");
  G4PVPlacement *LG_layer = new G4PVPlacement(0,G4ThreeVector(0,0,Z+2*ZGrease+ZLG),LG_log,"myLG",worldLV,0,false,0);


    //  Light guide -0 deg -----------------------------------------------------------------------------------------------------------
    //  G4double ZLG = 20*mm/2;
    //  G4double XLG = X1;
    //  G4Box *LG = new G4Box("myLG",XLG,XLG,ZLG);
    //  G4LogicalVolume *LG_log = new G4LogicalVolume(LG,PERSPEX,"LG_log");
    //  G4PVPlacement *LG_layer = new G4PVPlacement(0,G4ThreeVector(0,0,Z+ZGrease*2+ZLG),LG_log,"myLG",worldLV,0,false,0);

  //SiPM ------------------------------------------------------------------------------------------------------------
  G4double ZSiPM = 0.1*mm/2;
  G4double XSiPM = 6*mm/2;
  G4Box *mySiPM = new G4Box("mySiPM",XSiPM,XSiPM,ZSiPM);
  G4LogicalVolume *mySiPM_log = new G4LogicalVolume(mySiPM,QuartzWindow,"mySiPM");
  G4PVPlacement *mySiPM_layer = new G4PVPlacement(0,G4ThreeVector(0,0,Z+ZGrease*2+2*ZLG+ZSiPM),mySiPM_log,"mySiPM",worldLV,0,false,0);


       //SiPM -without LG -----------------------------------------------------------------------------------------------------------
     // G4double ZSiPM = 0.1*mm/2;
     // G4double XSiPM = 6*mm/2;
     // G4Box *mySiPM = new G4Box("mySiPM",XSiPM,XSiPM,ZSiPM);
     // G4LogicalVolume *mySiPM_log = new G4LogicalVolume(mySiPM,QuartzWindow,"mySiPM");
     // G4PVPlacement *mySiPM_layer = new G4PVPlacement(0,G4ThreeVector(0,0,Z+ZGrease*2+ZSiPM),mySiPM_log,"mySiPM",worldLV,0,false,0);


  //Surface Vikuiti--------------------------------------------------------------------------------------------------------------
  G4OpticalSurface* VikuitiSurface = new G4OpticalSurface("VikuitiSurface");
  VikuitiSurface->SetType(dielectric_metal);
  VikuitiSurface->SetPolish(polished);
  VikuitiSurface->SetModel(glisur);
  G4double RefEv[2] = {1.0*eV,10.0*eV};
  G4double efficiency[2] = {1.0,1.0};
  G4double reflectivity[2] = {0.987,0.987};
  G4MaterialPropertiesTable*myST1=new G4MaterialPropertiesTable();
  myST1->AddProperty("REFLECTIVITY",RefEv,reflectivity,2);
  myST1->AddProperty("EFFICIENCY",RefEv,efficiency,2);
  VikuitiSurface->SetMaterialPropertiesTable(myST1);
  //Surface Vikuiti---with Scintilator-----------------------------------------------------------------------------------------------------------
  G4LogicalBorderSurface*VikuitiBorderSurface = new G4LogicalBorderSurface("VikuitiBorderSurface",box_layer,worldPV,VikuitiSurface);
  //Surface Vikuiti---with Grease-----------------------------------------------------------------------------------------------------------
  G4LogicalBorderSurface*VikuitiBorderSurface_PMT = new G4LogicalBorderSurface("VikuitiBorderSurface",myGrease_layer,worldPV,VikuitiSurface);
  //Surface Vikuiti---with LG-----------------------------------------------------------------------------------------------------------
  G4LogicalBorderSurface*VikuitiBorderSurface_LG = new G4LogicalBorderSurface("VikuitiBorderSurface",LG_layer,worldPV,VikuitiSurface);

  // Visualization attributes
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  auto simpleGreaseVisAtt= new G4VisAttributes(G4Colour(1.0,0,1.0));
  simpleGreaseVisAtt->SetVisibility(true);
  myGrease_log->SetVisAttributes(simpleGreaseVisAtt);

  // Visualization attributes
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  auto simpleSiPMVisAtt= new G4VisAttributes(G4Colour(.0,0,1.0));
  simpleSiPMVisAtt->SetVisibility(true);
  mySiPM_log->SetVisAttributes(simpleSiPMVisAtt);

  // Visualization attributes Scintillator
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  box_log->SetVisAttributes(simpleBoxVisAtt);

  //Visualization attributes Light Guide
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());
  auto simpleLGVisAtt= new G4VisAttributes(G4Colour(0,1.0,0));
  simpleLGVisAtt->SetVisibility(true);
  LG_log->SetVisAttributes(simpleLGVisAtt);

  // Always return the physical World
  return worldPV;
}
