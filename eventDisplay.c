#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>

void calculateMuonPoints();
void calculateElectronPoints();
void AnimateMuon();
void AnimateElectron();

TRootEmbeddedCanvas *fECanvas;

TGCheckButton *draw_ecal;
TGCheckButton *draw_hcal;
TGCheckButton *draw_coil;
TGCheckButton *draw_chamber;

TGCheckButton *draw_muons, *draw_electrons, *draw_photons, *draw_protons;

TTimer *muTimer;
TTimer *eTimer;

// TGeoTrack 
TSPHE *proton1, *proton2;
TTUBE *parent;
TNode *parentNode;
TNode *proton_bunch1, *proton_bunch2;
int proton_step = 0;

int nMUON = 2;
int nELECTRON = 2;
int nPHOTON = 1;
int nPROTON = 8;

TGeoTrack *muonTrack[10];
TGeoTrack *electronTrack[10];
TGeoTrack *gammaTrack[10];

int t = 0;  // TGeoTrack time

TGeoTrack *protons[8];

class MyMainFrame {
	RQ_OBJECT("MyMainFrame")
	private:
   		TGMainFrame         *fMain;

	public:
   		MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
   		virtual ~MyMainFrame();
   		void DrawEcal();
		void DrawHcal();
		void DrawCoil();
		void DrawChamber();

      		void DrawMuMu(); 
		void DrawEE();

		void RhoPhi();
		void RZ();

		void DrawMuons();
		void DrawElectrons();
		void DrawPhotons();
		void DrawProtons();

		void Reset();
};


MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h) {
	// Create a main frame
   	fMain = new TGMainFrame(p,w,h);

   	// Create canvas widget
   	fECanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,1000,1000);
   	fMain->AddFrame(fECanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10,10,10,1));
   
	// Create a horizontal frame widget with buttons
   	TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40);

	draw_ecal = new TGCheckButton(hframe,"&ECAL");
   	draw_ecal->Connect("Clicked()","MyMainFrame",this,"DrawEcal()");
   	hframe->AddFrame(draw_ecal, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	draw_ecal->SetDown();

	draw_hcal = new TGCheckButton(hframe,"&HCAL");
   	draw_hcal->Connect("Clicked()","MyMainFrame",this,"DrawHcal()");
   	hframe->AddFrame(draw_hcal, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	draw_hcal->SetDown();
	
	draw_coil = new TGCheckButton(hframe,"&COIL");
   	draw_coil->Connect("Clicked()","MyMainFrame",this,"DrawCoil()");
   	hframe->AddFrame(draw_coil, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	draw_coil->SetDown();
	
	draw_chamber = new TGCheckButton(hframe,"&CHAMBER");
   	draw_chamber->Connect("Clicked()","MyMainFrame",this,"DrawChamber()");
   	hframe->AddFrame(draw_chamber, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	draw_chamber->SetDown();
 
	TGTextButton *draw_mumu = new TGTextButton(hframe, "&Z to mumu");
	draw_mumu->Connect("Clicked()", "MyMainFrame", this, "DrawMuMu()");
	hframe->AddFrame(draw_mumu, new TGLayoutHints(kLHintsCenterX,5,5,3,4)); 

	TGTextButton *draw_ee = new TGTextButton(hframe, "&Z to ee");
	draw_ee->Connect("Clicked()", "MyMainFrame", this, "DrawEE()");
	hframe->AddFrame(draw_ee, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

	TGTextButton *rhophi = new TGTextButton(hframe, "&RHO PHI view");
	rhophi->Connect("Clicked()", "MyMainFrame", this, "RhoPhi()");
	hframe->AddFrame(rhophi, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

	TGTextButton *rz = new TGTextButton(hframe, "&R Z view");
	rz->Connect("Clicked()", "MyMainFrame", this, "RZ()");
	hframe->AddFrame(rz, new TGLayoutHints(kLHintsCenterX, 5,5,3,4));	

	draw_muons = new TGCheckButton(hframe,"&MUON(Blue)");
   	draw_muons->Connect("Clicked()","MyMainFrame",this,"DrawMuons()");
   	hframe->AddFrame(draw_muons, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	draw_muons->SetDown();

	draw_electrons = new TGCheckButton(hframe,"&ELECTRON(Green)");
   	draw_electrons->Connect("Clicked()","MyMainFrame",this,"DrawElectrons()");
   	hframe->AddFrame(draw_electrons, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	draw_electrons->SetDown();

	draw_photons = new TGCheckButton(hframe,"&PHOTON(Red)");
   	draw_photons->Connect("Clicked()","MyMainFrame",this,"DrawPhotons()");
   	hframe->AddFrame(draw_photons, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	draw_photons->SetDown();

	draw_protons = new TGCheckButton(hframe,"&PROTON(Yellow)");
   	draw_protons->Connect("Clicked()","MyMainFrame",this,"DrawProtons()");
   	hframe->AddFrame(draw_protons, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	draw_protons->SetDown();

	TGTextButton *reset = new TGTextButton(hframe, "&Reset");
	reset->Connect("Clicked()", "MyMainFrame", this, "Reset()");
	hframe->AddFrame(reset, new TGLayoutHints(kLHintsCenterX, 5,5,3,4));
	

	TGTextButton *exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)");
   	hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  
	fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

   	// Set a name to the main frame
   	fMain->SetWindowName("Event Display");

   	// Map all subwindows of main frame
   	fMain->MapSubwindows();

   	// Initialize the layout algorithm
   	fMain->Resize(fMain->GetDefaultSize());

   	// Map main frame
   	fMain->MapWindow();
}


// calculate x,y,z coordinates of the helical path
Double_t *muX[10], *eX[10]; // 10 muons, electrons at max
Double_t *muY[10], *eY[10];
Double_t *muZ[10], *eZ[10];

const Int_t kNMAX = 100000; // for each X[i], there are 100000 points at max

double muRadius[10], eRadius[10];    // radius of the circle path
double muA[10], muB[10], eA[10], eB[10];  // center points of the circle path
int muSign[10][2], eSign[10][2];      // sign of A and B
	
int muDrawcursor[10], eDrawcursor[10];
int muStep[10], eStep[10];

Double_t *MuonX[10], *ElectronX[10], *muPhotonX[10], *ePhotonX[10]; // for 10 muons at max
Double_t *MuonY[10], *ElectronY[10], *muPhotonY[10], *ePhotonY[10];
Double_t *MuonZ[10], *ElectronZ[10], *muPhotonZ[10], *ePhotonZ[10];

double scalingFactorPhoton[10];

// detector hardware
double muonChamber = 700;
double ecalRadius = 100;
double hcalRadius = 280; 
double detectorRadius = 400; // 4m for now
double magneticField = 3.8;


// from root file
TFile *fMu;
TFile *fE;

TTree *tMu;
TTree *tE;

int Muon_Charge[2], Electron_Charge[10];
int NMuon, NElectron, NPhoton;

Double_t Muon_Px[10], Muon_Py[10], Muon_Pz[10], Muon_E[10];	
Double_t Electron_Px[10], Electron_Py[10], Electron_Pz[10];
Double_t Photon_Px[10], Photon_Py[10], Photon_Pz[10];


// select event
double min_Pt = 0;
int min_Pt_index = 0;
double max_pt = 0;
int max_pt_index;
int muEventNumber, eEventNumber;

// TGeometry
TGeoManager *geom;
TGeoMaterial *matVacuum;
TGeoMedium *medVacuum;

TGeoVolume *vol_top, *vol_super, *vol_ecal, *vol_hcal, *vol_muChamber;
TGeoVolume *super, *ecal, *hcal, *muChamber;

TGeoVolume *top;
TGeoVolume *vertex;

TView *view;
TCanvas *fCanvas;

Double_t *proton_px[8], *proton_py[8], *proton_pz[8];


void eventDisplay() {
  	calculateMuonPoints();
  	calculateElectronPoints();
 
	for (int j=0; j<nPROTON; j++) {

		proton_px[j] = new Double_t[kNMAX];
		proton_py[j] = new Double_t[kNMAX];
		proton_pz[j] = new Double_t[kNMAX];
	}

	for (int j=0; j<nPROTON; j++) {
		TFile *file1 = new TFile("./pileup/pileup.root");
		TFile *file2 = new TFile("./pileup/pileup2.root");
		TFile *file3 = new TFile("./pileup/pileup3.root");
		TFile *file4 = new TFile("./pileup/pileup4.root");

		TTree *tree1_1 = (TTree*)file1->Get("Proton1");
		TTree *tree1_2 = (TTree*)file1->Get("Proton2");
		
		tree1_1->SetBranchAddress("X", proton_px[0]);
		tree1_1->SetBranchAddress("Y", proton_py[0]);
		tree1_1->SetBranchAddress("Z", proton_pz[0]);

		tree1_2->SetBranchAddress("X", proton_px[1]);
		tree1_2->SetBranchAddress("Y", proton_py[1]);
		tree1_2->SetBranchAddress("Z", proton_pz[1]);

		tree1_1->GetEntry(0);
		tree1_2->GetEntry(0);

		TTree *tree2_1 = (TTree*)file2->Get("Proton1");
		TTree *tree2_2 = (TTree*)file2->Get("Proton2");
		
		tree2_1->SetBranchAddress("X", proton_px[2]);
		tree2_1->SetBranchAddress("Y", proton_py[2]);
		tree2_1->SetBranchAddress("Z", proton_pz[2]);

		tree2_2->SetBranchAddress("X", proton_px[3]);
		tree2_2->SetBranchAddress("Y", proton_py[3]);
		tree2_2->SetBranchAddress("Z", proton_pz[3]);

		tree2_1->GetEntry(0);
		tree2_2->GetEntry(0);

		TTree *tree3_1 = (TTree*)file3->Get("Proton1");
		TTree *tree3_2 = (TTree*)file3->Get("Proton2");
		
		tree3_1->SetBranchAddress("X", proton_px[4]);
		tree3_1->SetBranchAddress("Y", proton_py[4]);
		tree3_1->SetBranchAddress("Z", proton_pz[4]);

		tree3_2->SetBranchAddress("X", proton_px[5]);
		tree3_2->SetBranchAddress("Y", proton_py[5]);
		tree3_2->SetBranchAddress("Z", proton_pz[5]);

		tree3_1->GetEntry(0);
		tree3_2->GetEntry(0);

		TTree *tree4_1 = (TTree*)file4->Get("Proton1");
		TTree *tree4_2 = (TTree*)file4->Get("Proton2");
		
		tree4_1->SetBranchAddress("X", proton_px[6]);
		tree4_1->SetBranchAddress("Y", proton_py[6]);
		tree4_1->SetBranchAddress("Z", proton_pz[6]);

		tree4_2->SetBranchAddress("X", proton_px[7]);
		tree4_2->SetBranchAddress("Y", proton_py[7]);
		tree4_2->SetBranchAddress("Z", proton_pz[7]);

		tree4_1->GetEntry(0);
		tree4_2->GetEntry(0);
		

	}
	
	// Popup the GUI...
   	new MyMainFrame(gClient->GetRoot(),200,200);

	fCanvas = fECanvas->GetCanvas();
	fCanvas->SetFillColor(kBlack);

        geom = new TGeoManager("tube", "poza2");

	// material and medium
        matVacuum = new TGeoMaterial("Empty Space", 0, 0, 0);
        medVacuum = new TGeoMedium("MED",1,matVacuum);

	// top container
	vol_top = geom->MakeBox("TOP", medVacuum, 1000, 1000, 1500);
	geom->SetTopVolume(vol_top);

        // superconducting coil container volume
        vol_super = geom->MakeBox("SUPER",medVacuum,1000,1000,1000);
        //geom->SetTopVolume(vol_super);

        // name, material, rmin, rmax, dz       
        super = geom->MakeTube("superTUBE",medVacuum, 300,400,1000);
        super->SetLineWidth(1);
        super->SetLineColor(kGray+4);

        // electromagnetic calorimeter container volume
        vol_ecal = geom->MakeBox("ECAL", medVacuum, 90, 100, 1000);

        ecal = geom->MakeTube("ecalTUBE", medVacuum, 90, 100, 1000);
        ecal->SetLineWidth(1);
        ecal->SetLineColor(kGray);

	// hadron calorimeter
	vol_hcal = geom->MakeBox("HCAL", medVacuum, 120, 280, 1000);
	hcal = geom->MakeTube("hcalTUBE", medVacuum, 120, 280, 1000);
	hcal->SetLineWidth(1);
	hcal->SetLineColor(kMagenta-8);

	// muon chamber
	vol_muChamber = geom->MakeBox("muonChamber", medVacuum, 450, 700, 1500);
	muChamber = geom->MakeTube("muonChamberTUBE", medVacuum, 450, 700, 1500);
	muChamber->SetLineWidth(1);
	muChamber->SetLineColor(kRed+2);

        // volume, copy_no
        vol_super->AddNode(super,1);
        vol_ecal->AddNode(ecal,1);
	vol_hcal->AddNode(hcal,1);       
	vol_muChamber->AddNode(muChamber,1);
 
        // # of segments for approximating circles in drawing
        geom->SetNsegments(20);
	vol_top->Draw();
	vol_super->Draw("SAME");
        super->Draw("SAME");
	ecal->Draw("SAME");
	hcal->Draw("SAME");
	muChamber->Draw("SAME");

        view = gPad->GetView();
        //view->ShowAxis();

}


void MyMainFrame::DrawEcal() {

	if ( draw_ecal->IsDown() == false ) { 
		vol_top->Draw();	
		if ( draw_coil->IsDown() == true ) { super->Draw("SAME"); }	
		if ( draw_hcal->IsDown() == true ) { hcal->Draw("SAME"); }
		if ( draw_chamber->IsDown() == true ) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();
	}
	else { 
		vol_top->Draw();
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}


		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();
	}
}


void MyMainFrame::DrawHcal() {

	if ( draw_hcal->IsDown() == false ) { 
		vol_top->Draw();	
		if ( draw_coil->IsDown() == true ) { super->Draw("SAME"); }	
		if ( draw_ecal->IsDown() == true ) { ecal->Draw("SAME"); }
		if ( draw_chamber->IsDown() == true ) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();
	}
	else { 
		vol_top->Draw();
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}


		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();
	}

}


void MyMainFrame::DrawCoil() {

	if ( draw_coil->IsDown() == false ) { 
		vol_top->Draw();	
		if ( draw_hcal->IsDown() == true ) { hcal->Draw("SAME"); }	
		if ( draw_ecal->IsDown() == true ) { ecal->Draw("SAME"); }
		if ( draw_chamber->IsDown() == true ) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();
	}
	else { 
		vol_top->Draw();
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}


		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();
	}
}

void MyMainFrame::DrawChamber() {
	if ( draw_chamber->IsDown() == false ) { 
		vol_top->Draw();	
		if ( draw_hcal->IsDown() == true ) { hcal->Draw("SAME"); }	
		if ( draw_ecal->IsDown() == true ) { ecal->Draw("SAME"); }
		if ( draw_coil->IsDown() == true ) { super->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();
	}
	else { 
		vol_top->Draw();
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}


		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();
	}

}

void MyMainFrame::DrawMuMu() {

	t = 0;

	cout << "DRAW Z TO MUMU! \n" << endl;

	cout << "\nEVENT: " << muEventNumber << " / 10000\n" << endl;

	for (int j=0; j<nMUON; j++) {
		if (Muon_Charge[j] == -1) { cout << "MUON " << j+1 << " is muon. " << endl; }
		else if (Muon_Charge[j] == 1) { cout << "MUON " << j+1 << " is antimuon. " << endl; }
		cout << "MUON " << j+1 << " pT = " << sqrt(Muon_Px[j]*Muon_Px[j]+Muon_Py[j]*Muon_Py[j]) << " GeV\n" << endl;
	}


	if (proton_bunch1 != NULL ) { delete proton_bunch1; proton_bunch1 = NULL; }
	if (proton_bunch2 != NULL ) { delete proton_bunch2; proton_bunch2 = NULL; }

	if (proton1 != NULL ) { delete proton1; proton1 = NULL; }
	if (proton2 != NULL ) { delete proton2; proton2 = NULL; }

	if (parentNode != NULL ) { delete parentNode; parentNode = NULL; }
	if (parent != NULL ) { delete parent; parent = NULL; }
	proton_step = 0;
	

	parent = new TTUBE("PARENT", "SPHE", "void", 1000, 1000, 1000);
	proton1 = new TSPHE("PROTONBUNCH1", "SPHE", "void", 0, 15, 0, 180, 0, 360);
	proton2 = new TSPHE("PROTONBUNCH2", "SPHE", "void", 0, 15, 0, 180, 0, 360);

	parentNode = new TNode("PARENTNODE", "PARENTNODE", "PARENT");
	parentNode->cd();
	proton_bunch1 = new TNode("NODE1", "NODE1", "PROTONBUNCH1");
	proton_bunch2 = new TNode("NODE2", "NODE2", "PROTONBUNCH2");
	proton_bunch1->SetPosition(0,0,2000);
	proton_bunch2->SetPosition(0,0,-2000);
	proton_bunch1->SetLineColor(kYellow);
	proton_bunch2->SetLineColor(kYellow);
	parentNode->Draw("SAME");

	for (int j=0; j<nPROTON; j++) {
		protons[j] = new TGeoTrack(3,3);
	}

	for (int i=0; i<nMUON; i++) {
		muonTrack[i] = new TGeoTrack(1,13);
	}

	for (int i=0; i<nPHOTON; i++) {
		gammaTrack[i] = new TGeoTrack(1,13);
	}

	if (eTimer != NULL) { delete eTimer; eTimer = NULL; }

	muTimer = new TTimer(1);
	muTimer->SetCommand("AnimateMuon()");
	muTimer->TurnOn();

}


void MyMainFrame::DrawEE() {
	
	t = 0;

	cout << "DRAW Z TO EE! \n" << endl;

	cout << "\nEVENT: " << eEventNumber << " / 10000\n" << endl;

	for (int j=0; j<nELECTRON; j++) {
		if (Electron_Charge[j] == -1) { cout << "ELECTRON " << j+1 << " is electron. " << endl; }
		else if (Electron_Charge[j] == 1) { cout << "ELECTRON" << j+1 << " is positron. " << endl; }
		cout << "ELECTRON " << j+1 << " pT = " << sqrt(Electron_Px[j]*Electron_Px[j]+Electron_Py[j]*Electron_Py[j]) << " GeV\n" << endl;
	}



	if (proton_bunch1 != NULL ) { delete proton_bunch1; proton_bunch1 = NULL; }
	if (proton_bunch2 != NULL ) { delete proton_bunch2; proton_bunch2 = NULL; }

	if (proton1 != NULL ) { delete proton1; proton1 = NULL; }
	if (proton2 != NULL ) { delete proton2; proton2 = NULL; }

	if (parentNode != NULL ) { delete parentNode; parentNode = NULL; }
	if (parent != NULL ) { delete parent; parent = NULL; }
	proton_step = 0;
	
	parent = new TTUBE("PARENT", "SPHE", "void", 1000, 1000, 1000);
	proton1 = new TSPHE("PROTONBUNCH1", "SPHE", "void", 0, 15, 0, 180, 0, 360);
	proton2 = new TSPHE("PROTONBUNCH2", "SPHE", "void", 0, 15, 0, 180, 0, 360);

	parentNode = new TNode("PARENTNODE", "PARENTNODE", "PARENT");
	parentNode->cd();
	proton_bunch1 = new TNode("NODE1", "NODE1", "PROTONBUNCH1");
	proton_bunch2 = new TNode("NODE2", "NODE2", "PROTONBUNCH2");
	proton_bunch1->SetPosition(0,0, 2000);
	proton_bunch2->SetPosition(0,0,-2000);
	proton_bunch1->SetLineColor(kWhite);
	proton_bunch2->SetLineColor(kWhite);
	parentNode->Draw("SAME");


	if (muTimer != NULL) { delete muTimer; muTimer = NULL; }
	
	for (int j=0; j<nPROTON; j++) {
		protons[j] = new TGeoTrack(3,3);
	}

	for (int i=0; i<nELECTRON; i++) {
		electronTrack[i] = new TGeoTrack(1,13);
	}


	for (int i=0; i<nPHOTON; i++) {
		gammaTrack[i] = new TGeoTrack(1,13);
	}

	eTimer = new TTimer(1);
	eTimer->SetCommand("AnimateElectron()");
	eTimer->TurnOn();

}


void MyMainFrame::RhoPhi() {

	gPad->SetTheta(90);
	gPad->Modified();
	gPad->Update();

}


void MyMainFrame::RZ() {

	gPad->SetTheta(0);
	gPad->Modified();
	gPad->Update();

}


void MyMainFrame::DrawMuons() {

	if ( draw_muons->IsDown() == false ) { 
		cout << "NOT DRAW MUON" << endl;
		vol_top->Draw();	
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }	
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true ) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();
		
	}
	else {
		cout << "DRAW MUON " << endl; 
		vol_top->Draw();	
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }	
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }


		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();
	}

}


void MyMainFrame::DrawElectrons() {

	if ( draw_electrons->IsDown() == false ) { 
		cout << "NOT DRAW ELECTRON " << endl; 
		vol_top->Draw();	
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }	
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true ) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();

	}
	else { 

		cout << "DRAW ELECTRONS " << endl;
		vol_top->Draw();	
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }	
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }


		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();

	}
}


void MyMainFrame::DrawPhotons() {

	if ( draw_photons->IsDown() == false ) { 
		cout << "NOT DRAW PHOTON" << endl; 
		vol_top->Draw();	
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }	
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true ) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();

	}
	else { 

		cout << "DRAW PHOTONS " << endl;
		vol_top->Draw();	
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }	
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true ) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();

	}
}


void MyMainFrame::DrawProtons() {

	if ( draw_protons->IsDown() == false ) { 
		cout << "NOT DRAW PROTON" << endl; 
		vol_top->Draw();	
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }	
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true ) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true) { gammaTrack[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();

	}
	else { 

		cout << "DRAW PROTONS " << endl;
		vol_top->Draw();	
		if (draw_ecal->IsDown() == true) { ecal->Draw("SAME"); }
		if (draw_coil->IsDown() == true) { super->Draw("SAME"); }	
		if (draw_hcal->IsDown() == true) { hcal->Draw("SAME"); }
		if (draw_chamber->IsDown() == true) { muChamber->Draw("SAME"); }

		for (int i=0; i<nMUON; i++) {
			if (muonTrack[i] != NULL && draw_muons->IsDown() == true) { muonTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nELECTRON; i++) {
			if (electronTrack[i] != NULL && draw_electrons->IsDown() == true) { electronTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPHOTON; i++) {
			if (gammaTrack[i] != NULL && draw_photons->IsDown() == true ) { gammaTrack[i]->Draw("SAME"); }
		}

		for (int i=0; i<nPROTON; i++) {
			if (protons[i] != NULL && draw_protons->IsDown() == true) { protons[i]->Draw("SAME"); }
		}

		fCanvas = fECanvas->GetCanvas();
		fCanvas->cd();
		fCanvas->Update();

	}
}




void MyMainFrame::Reset() {

	draw_ecal->SetDown();
	draw_hcal->SetDown();
	draw_coil->SetDown();
	draw_chamber->SetDown();
	
	draw_muons->SetDown();			
	draw_electrons->SetDown();
	draw_photons->SetDown();
	draw_protons->SetDown();

	vol_top->Draw();
	super->Draw("SAME");
	ecal->Draw("SAME");
	hcal->Draw("SAME");
	muChamber->Draw("SAME");

	if (eTimer != NULL) { delete eTimer; eTimer = NULL; }
	if (muTimer != NULL) { delete muTimer; muTimer = NULL; }

	for (int i=0; i<nMUON; i++) {
		if (muonTrack[i] != NULL) { muonTrack[i] = NULL; }
	}


	for (int i=0; i<nELECTRON; i++) {
		if (electronTrack[i] != NULL) { electronTrack[i] = NULL; }
	}

	for (int i=0; i<nPHOTON; i++) {
		if (gammaTrack[i] != NULL) { gammaTrack[i] = NULL; }
	}
	
	for (int i=0; i<nPROTON; i++) {
		if (protons[i] != NULL) { protons[i] = NULL; }
	}

	if (proton_bunch1 != NULL ) { delete proton_bunch1; proton_bunch1 = NULL; }
	if (proton_bunch2 != NULL ) { delete proton_bunch2; proton_bunch2 = NULL; }

	if (proton1 != NULL ) { delete proton1; proton1 = NULL; }
	if (proton2 != NULL ) { delete proton2; proton2 = NULL; }

	if (parentNode != NULL ) { delete parentNode; parentNode = NULL; }
	if (parent != NULL ) { delete parent; parent = NULL; }
	proton_step = 0;
	
	fCanvas = fECanvas->GetCanvas();
	fCanvas->cd();
	fCanvas->Update();
	
}


void calculateMuonPoints() {

  	fMu = new TFile("./delphes/zmumu.root");
	tMu = (TTree*)fMu->Get("data");

	int muEntries = tMu->GetEntries();
	int dimuon_entries = 0;

	tMu->SetBranchAddress("Photon_Px", &Photon_Px);
	tMu->SetBranchAddress("Photon_Py", &Photon_Py);
	tMu->SetBranchAddress("Photon_Pz", &Photon_Pz);
	tMu->SetBranchAddress("NPhoton", &NPhoton);

	tMu->SetBranchAddress("Muon_Px", &Muon_Px);
	tMu->SetBranchAddress("Muon_Py", &Muon_Py);
	tMu->SetBranchAddress("Muon_Pz", &Muon_Pz);
	tMu->SetBranchAddress("NMuon", &NMuon);
	tMu->SetBranchAddress("Muon_Charge", &Muon_Charge);

	int nloop = 0;
	for (int i=0; i<muEntries; i++) {
		tMu->GetEntry(i);
		if (NMuon>1) {
			nloop += 1;
			dimuon_entries += 1;
		}	
	}
	cout << "\n# of total entries " << muEntries << endl;
	cout << "# of dimuon entries " << dimuon_entries << endl;

	int nLoop = 0;
	for (int i=0; i<muEntries; i++) {
		tMu->GetEntry(i);
		if ( NMuon == nMUON ) {
			if ( NPhoton == 1) { 
				nLoop += 1;
				muEventNumber = i; 
				if ( nLoop == 10 ) { break; }
			}
		}
	}

	tMu->GetEntry(muEventNumber);

	for (int i=0; i<nMUON; i++) {
		if (Muon_Charge[i] == 1) { 
			cout << "Muon " << i+1 << " is antimuon " << endl; 
		}
		else if (Muon_Charge[i] == -1) { 
			cout << "Muon " << i+1 << " is muon " << endl; 
		}
	}


	for (int i=0; i<nMUON; i++) {
		muRadius[i] = 1*(detectorRadius/3)*sqrt(Muon_Px[i]*Muon_Px[i]+Muon_Py[i]*Muon_Py[i])/magneticField;
	}
	
	for (int i=0; i<nMUON; i++) {
		cout << "Muon " << i+1 << ": px = " << Muon_Px[i] << ", py = " << Muon_Py[i] << ",  pz = " << Muon_Pz[i] << endl;
		cout << "radius = " << sqrt(Muon_Px[i]*Muon_Px[i]+Muon_Py[i]*Muon_Py[i])/(magneticField*1) << ", radius in canvas scale = " << muRadius[i] << endl;
	}

	
	for (int i=0; i<nMUON; i++) {
		if (Muon_Px[i] > 0 && Muon_Py[i] > 0) {
			if (Muon_Charge[i] == 1) {
				muSign[i][0] = 1;
				muSign[i][1] = -1;
			}
			else if (Muon_Charge[i] == -1) {
				muSign[i][0] = -1;
				muSign[i][1] = 1;
			}
		}
		else if (Muon_Px[i] > 0 && Muon_Py[i] < 0) {
			if (Muon_Charge[i] == 1) {
				muSign[i][0] = -1;
				muSign[i][1] = -1;
			}
			else if (Muon_Charge[i] == -1) {
				muSign[i][0] = 1;
				muSign[i][1] = 1;
			}
		}
		else if (Muon_Px[i] < 0 && Muon_Py[i] > 0) {
			if (Muon_Charge[i] == 1) {
				muSign[i][0] = 1;
				muSign[i][1] = 1;
			}
			else if (Muon_Charge[i] == -1) {
				muSign[i][0] = -1;
				muSign[i][1] = -1;
			}
		}
		else if (Muon_Px[i] < 0 && Muon_Py[i] < 0) {
			if (Muon_Charge[i] == 1) {
				muSign[i][0] = -1;
				muSign[i][1] = 1;
			}
			else if (Muon_Charge[i] == -1) {
				muSign[i][0] = 1;
				muSign[i][1] = -1;
			}		
		}
	}

	for (int i=0; i<nMUON; i++) {
		muA[i] = muSign[i][0]*sqrt((muRadius[i]*muRadius[i])/(1+((Muon_Px[i]*Muon_Px[i])/(Muon_Py[i]*Muon_Py[i]))));
		muB[i] = muSign[i][1]*abs((Muon_Px[i]/Muon_Py[i])*muA[i]);
		//cout << "Muon " << i+1 << ": x,y " << muA[i] << ", " << muB[i] << endl;
	}


	for (int i=0; i<nMUON; i++) {
		muX[i] = new Double_t[kNMAX];
		muY[i] = new Double_t[kNMAX];
		muZ[i] = new Double_t[kNMAX];

	}

	for (int j=0; j<nMUON; j++) {
		for (int i=0; i<kNMAX; i++) {
			muX[j][i] = 0;
			muY[j][i] = 0;
			muZ[j][i] = 0;
		}
	}	

	for (int j=0; j<nMUON; j++) {
		for (int i=0; i<4*muRadius[j]; i++) {
			muZ[j][i] = abs(1/Muon_Px[j])*Muon_Pz[j]*i;
		
		}
	}

	for (int j=0; j<nMUON; j++) {
		muX[j][0] = muA[j];
		muY[j][0] = muB[j] + sqrt(muRadius[j]*muRadius[j]-muStep[j]*muStep[j]);		
		
		for (int i=1; i<muRadius[j]; i++) {
			muStep[j] += 1;
			if (muRadius[j]*muRadius[j]>=muStep[j]*muStep[j]){
				muX[j][i] = muA[j]+muStep[j];
				muY[j][i] = muB[j]+sqrt(muRadius[j]*muRadius[j]-muStep[j]*muStep[j]);
				//if ( std::isnan(muY[j][i]) == 1 ) { cout << "NAN " << i << " / " << muRadius[j] << ", " << muX[j][i] << ", " << muY[j][i] << endl; }
			}
			else {
				muX[j][i] = muA[j]+muStep[j];
				muY[j][i] = muY[j][i-1];
			}
			
		}

		for (int i=muRadius[j]; i<2*muRadius[j]; i++) {
			muStep[j] -= 1;
		
			if (muRadius[j]*muRadius[j]>= muStep[j]*muStep[j]){	
				muX[j][i] = muA[j]+muStep[j];
				muY[j][i] = muB[j]-sqrt(muRadius[j]*muRadius[j]-muStep[j]*muStep[j]);
				//if ( std::isnan(muY[j][i]) == 1 ) { cout << "NAN " << i << " / " << 2*muRadius[j] << ", " << muX[j][i] << ", " << muY[j][i] << endl; }
			}
			else {
				muX[j][i] = muA[j]+muStep[j];
				muY[j][i] = muY[j][i-1];
			}
		
		}

		for (int i=2*muRadius[j]; i<3*muRadius[j]; i++) {
			muStep[j] -= 1;
			
			if (muRadius[j]*muRadius[j] >= muStep[j]*muStep[j]) {
				muX[j][i] = muA[j]+muStep[j];
				muY[j][i] = muB[j]-sqrt(muRadius[j]*muRadius[j]-muStep[j]*muStep[j]);
				//if ( std::isnan(muY[j][i]) == 1 ) { cout << "NAN " << i << " / " << 3*muRadius[j] << ", " << muX[j][i] << ", " << muY[j][i] << endl; }
			}
			else {
				muX[j][i] = muA[j]+muStep[j];
				muY[j][i] = muY[j][i-1];
			}
		
		}

		for (int i=3*muRadius[j]; i<4*muRadius[j]; i++) {
			muStep[j] += 1;
			
			if (muRadius[j]*muRadius[j] >= muStep[j]*muStep[j]) {
				muX[j][i] = muA[j]+muStep[j];
				muY[j][i] = muB[j]+sqrt(muRadius[j]*muRadius[j]-muStep[j]*muStep[j]);
				//if ( std::isnan(muY[j][i]) == 1 ) { cout << "NAN " << i << " / " << 4*muRadius[j] << ", " << muX[j][i] << ", " << muY[j][i] << endl; }
			}
			else {
				muX[j][i] = muA[j]+muStep[j];
				muY[j][i] = muY[j][i-1];
			}
	
		}

		for (int i=0; i<4*muRadius[j]; i++) {
			if ( muX[j][i]*muX[j][i] + muY[j][i]*muY[j][i] <= muX[j][muDrawcursor[j]]*muX[j][muDrawcursor[j]] + muY[j][muDrawcursor[j]]*muY[j][muDrawcursor[j]] ) {
				muDrawcursor[j] = i;
			}

		}
		//cout << "muDrawcursor " << j+1 << ": " << muDrawcursor[j] << " / " << 4*muRadius[j] << endl;
		//cout << muX[j][muDrawcursor[j]] << ", " << muY[j][muDrawcursor[j]] << endl;
	
	}

	for (int l=0; l<nMUON; l++) {
		muX[l][int(4*muRadius[l])] = muX[l][int(4*muRadius[l])-1];
		muY[l][int(4*muRadius[l])] = muY[l][int(4*muRadius[l])-1];
	}


	// points
	for (int k=0; k<nMUON; k++) {

		MuonX[k] = new Double_t[kNMAX];
		MuonY[k] = new Double_t[kNMAX];
		MuonZ[k] = new Double_t[kNMAX];

		MuonX[k][0] = muX[k][muDrawcursor[k]];
		MuonY[k][0] = muY[k][muDrawcursor[k]];
		MuonZ[k][0] = 0;

	}


	for (int i=0; i<kNMAX-1; i++) {
		
		for (int j=0; j<nMUON; j++) {

			if ( MuonX[j][i]*MuonX[j][i] + MuonY[j][i]*MuonY[j][i] < muonChamber*muonChamber ) {	
				// muon
				if (Muon_Charge[j] == -1) {
					// px > 0, py > 0 ( A < 0, B > 0 )
					if ( muA[j] < 0 && muB[j] > 0 ) {
						if ( muDrawcursor[j] == 0 ) { muDrawcursor[j] = int(4*muRadius[j]); }
						muDrawcursor[j]--;
	 				}
	     				// px > 0, py < 0 ( A > 0, B > 0 )
			      		else if ( muA[j] > 0 && muB[j] > 0 ) {
						if ( muDrawcursor[j] == 0 ) { muDrawcursor[j] = int(4*muRadius[j]); }
						muDrawcursor[j]--;
			       		}
			     		// px < 0, py < 0 ( A > 0 , B < 0 )
			      		else if ( muA[j] > 0 && muB[j] < 0 ) {
						if ( muDrawcursor[j] == 0 ) { muDrawcursor[j] = int(4*muRadius[j]); }
						muDrawcursor[j]--;
			   		}
			   		// px < 0, py > 0 ( A < 0, B < 0 ) 
					else if ( muA[j] < 0 && muB[j] < 0 ) {
						if ( muDrawcursor[j] == 0 ) { muDrawcursor[j] = int(4*muRadius[j]); }
						muDrawcursor[j]--;
					}
				}

     	  			// anti muon
				// antimuon: if X[j][muDrawcursor[j]+1] == Y[j][muDrawcursor[j]+1] == 0: muDrawcursor[j] = 0;
	    			else if (Muon_Charge[j] == 1) {
	      				// px > 0, py > 0 ( A > 0, B < 0 )
	      				if ( muA[j] > 0 && muB[j] < 0 ) {
						if (muX[j][muDrawcursor[j]+1] == 0 && muY[j][muDrawcursor[j]+1] == 0 ) { muDrawcursor[j] = 0; }
						muDrawcursor[j]++;
	      				}
		      			// px > 0, py < 0 ( A < 0, B < 0 )
		      			else if ( muA[j] < 0 && muB[j] < 0 ) {
						if (muX[j][muDrawcursor[j]+1] == 0 && muY[j][muDrawcursor[j]+1] == 0 ) { muDrawcursor[j] = 0; }
						muDrawcursor[j]++;
		     		 	}
		      			// px < 0, py < 0 ( A < 0 , B > 0 )
				      	else if ( muA[j] < 0 && muB[j] > 0 ) {
						if (muX[j][muDrawcursor[j]+1] == 0 && muY[j][muDrawcursor[j]+1] == 0 ) { muDrawcursor[j] = 0; }
						muDrawcursor[j]++;
				      	}
				     	// px < 0, py > 0 ( A > 0, B > 0 ) 
				      	else if ( muA[j] > 0 && muB[j] > 0 ) {
						if (muX[j][muDrawcursor[j]+1] == 0 && muY[j][muDrawcursor[j]+1] == 0 ) { muDrawcursor[j] = 0; }
						muDrawcursor[j]++;
		      			}

		    		}

				MuonZ[j][i+1] = abs(1/Muon_Px[j])*Muon_Pz[j]*i;
			}
			else { MuonZ[j][i+1] = MuonZ[j][i]; }		

			MuonX[j][i+1] = muX[j][muDrawcursor[j]];
			MuonY[j][i+1] = muY[j][muDrawcursor[j]];

		}

	}

	for (int j=0; j<nPHOTON; j++) {
		muPhotonX[j] = new Double_t[kNMAX];
		muPhotonY[j] = new Double_t[kNMAX];
		muPhotonZ[j] = new Double_t[kNMAX];
	}
	
	for (int j=0; j<nPHOTON; j++) {
		//scalingFactorPhoton[j] = std::max(abs(Photon_Px[j]), std::max(abs(Photon_Py[j]), abs(Photon_Pz[j])));
		scalingFactorPhoton[j] = (1/Muon_Px[j]);		

		for (int i=0; i<kNMAX; i++) {
			muPhotonX[j][i] = (i+1)*Photon_Px[j]*scalingFactorPhoton[j];
			muPhotonY[j][i] = (i+1)*Photon_Py[j]*scalingFactorPhoton[j];
			muPhotonZ[j][i] = (i+1)*Photon_Pz[j]*scalingFactorPhoton[j];
		}
		cout << "Photon px = " << muPhotonX[0][0] << ", py = " << muPhotonY[0][0] << ", pz = " << muPhotonZ[0][0] << endl;
	} 

}


void calculateElectronPoints() {

  	fE = new TFile("./delphes/ztoee.root");
	tE = (TTree*)fE->Get("data");

	int eEntries = tE->GetEntries();
	int electron_entries = 0;
	cout << "eEntries " << eEntries << endl;

	tE->SetBranchAddress("Photon_Px", &Photon_Px);
	tE->SetBranchAddress("Photon_Py", &Photon_Py);
	tE->SetBranchAddress("Photon_Pz", &Photon_Pz);
	tE->SetBranchAddress("NPhoton", &NPhoton);

	tE->SetBranchAddress("Electron_Px", &Electron_Px);
	tE->SetBranchAddress("Electron_Py", &Electron_Py);
	tE->SetBranchAddress("Electron_Pz", &Electron_Pz);
	tE->SetBranchAddress("NElectron", &NElectron);
	tE->SetBranchAddress("Electron_Charge", &Electron_Charge);

	int nloop = 0;
	for (int i=0; i<eEntries; i++) {
		tE->GetEntry(i);
		if (NElectron != 0) {
			nloop += 1;
			electron_entries += 1;
		}	
	}
	cout << "\n# of total entries " << eEntries << endl;
	cout << "# of electron entries " << electron_entries << endl;

	for (int i=0; i<eEntries; i++) {
		tE->GetEntry(i);
		if ( NElectron == nELECTRON && NPhoton == nPHOTON ) {
			eEventNumber = i; break; }
		}

	tE->GetEntry(eEventNumber);
	cout << "ELECTRON EVENT # " << eEventNumber << endl;

	for (int i=0; i<nELECTRON; i++) {
		if (Electron_Charge[i] == 1) { 
			cout << "Electron " << i+1 << " is positron " << endl; 
		}
		else if (Electron_Charge[i] == -1) { 
			cout << "Electron " << i+1 << " is electron " << endl; 
		}
	}


	for (int i=0; i<nELECTRON; i++) {
		eRadius[i] = 1*(detectorRadius/4)*sqrt(Electron_Px[i]*Electron_Px[i]+Electron_Py[i]*Electron_Py[i])/magneticField;
	}
	
	for (int i=0; i<nELECTRON; i++) {
		cout << "Electron " << i+1 << ": px = " << Electron_Px[i] << ", py = " << Electron_Py[i] << ", pz = " << Electron_Pz[i] << endl;
		cout << "radius = " << sqrt(Electron_Px[i]*Electron_Px[i]+Electron_Py[i]*Electron_Py[i])/(magneticField*1) << ", radius in canvas scale = " << eRadius[i] << endl;
	}

	
	for (int i=0; i<nELECTRON; i++) {
		if (Electron_Px[i] > 0 && Electron_Py[i] > 0) {
			if (Electron_Charge[i] == 1) {
				eSign[i][0] = 1;
				eSign[i][1] = -1;
			}
			else if (Electron_Charge[i] == -1) {
				eSign[i][0] = -1;
				eSign[i][1] = 1;
			}
		}
		else if (Electron_Px[i] > 0 && Electron_Py[i] < 0) {
			if (Electron_Charge[i] == 1) {
				eSign[i][0] = -1;
				eSign[i][1] = -1;
			}
			else if (Electron_Charge[i] == -1) {
				eSign[i][0] = 1;
				eSign[i][1] = 1;
			}
		}
		else if (Electron_Px[i] < 0 && Electron_Py[i] > 0) {
			if (Electron_Charge[i] == 1) {
				eSign[i][0] = 1;
				eSign[i][1] = 1;
			}
			else if (Electron_Charge[i] == -1) {
				eSign[i][0] = -1;
				eSign[i][1] = -1;
			}
		}
		else if (Electron_Px[i] < 0 && Electron_Py[i] < 0) {
			if (Electron_Charge[i] == 1) {
				eSign[i][0] = -1;
				eSign[i][1] = 1;
			}
			else if (Electron_Charge[i] == -1) {
				eSign[i][0] = 1;
				eSign[i][1] = -1;
			}		
		}
	}

	for (int i=0; i<nELECTRON; i++) {
		eA[i] = eSign[i][0]*sqrt((eRadius[i]*eRadius[i])/(1+((Electron_Px[i]*Electron_Px[i])/(Electron_Py[i]*Electron_Py[i]))));
		eB[i] = eSign[i][1]*abs((Electron_Px[i]/Electron_Py[i])*eA[i]);
		//cout << "Electron " << i+1 << ": x,y " << eA[i] << ", " << eB[i] << endl;
	}


	for (int i=0; i<nELECTRON; i++) {
		eX[i] = new Double_t[kNMAX];
		eY[i] = new Double_t[kNMAX];
		eZ[i] = new Double_t[kNMAX];

	}

	for (int j=0; j<nELECTRON; j++) {
		for (int i=0; i<kNMAX; i++) {
			eX[j][i] = 0;
			eY[j][i] = 0;
			eZ[j][i] = 0;
		}
	}	

	for (int j=0; j<nELECTRON; j++) {
		//cout << "j " << j << endl;
		for (int i=0; i<4*eRadius[j]; i++) {
			eZ[j][i] = abs(1/Electron_Px[j])*Electron_Pz[j]*i;
		
		}
	}

	for (int j=0; j<nELECTRON; j++) {
		eX[j][0] = eA[j];
		eY[j][0] = eB[j] + sqrt(eRadius[j]*eRadius[j]-eStep[j]*eStep[j]);		
		
		for (int i=1; i<eRadius[j]; i++) {
			eStep[j] += 1;
			if (eRadius[j]*eRadius[j]>=eStep[j]*eStep[j]){
				eX[j][i] = eA[j]+eStep[j];
				eY[j][i] = eB[j]+sqrt(eRadius[j]*eRadius[j]-eStep[j]*eStep[j]);
				//if ( std::isnan(eY[j][i]) == 1 ) { cout << "NAN " << i << " / " << eRadius[j] << ", " << eX[j][i] << ", " << eY[j][i] << endl; }
			}
			else {
				eX[j][i] = eA[j]+eStep[j];
				eY[j][i] = eY[j][i-1];
			}
			
		}

		for (int i=eRadius[j]; i<2*eRadius[j]; i++) {
			eStep[j] -= 1;
		
			if (eRadius[j]*eRadius[j]>= eStep[j]*eStep[j]){	
				eX[j][i] = eA[j]+eStep[j];
				eY[j][i] = eB[j]-sqrt(eRadius[j]*eRadius[j]-eStep[j]*eStep[j]);
				//if ( std::isnan(eY[j][i]) == 1 ) { cout << "NAN " << i << " / " << 2*eRadius[j] << ", " << eX[j][i] << ", " << eY[j][i] << endl; }
			}
			else {
				eX[j][i] = eA[j]+eStep[j];
				eY[j][i] = eY[j][i-1];
			}
		
		}

		for (int i=2*eRadius[j]; i<3*eRadius[j]; i++) {
			eStep[j] -= 1;
			
			if (eRadius[j]*eRadius[j] >= eStep[j]*eStep[j]) {
				eX[j][i] = eA[j]+eStep[j];
				eY[j][i] = eB[j]-sqrt(eRadius[j]*eRadius[j]-eStep[j]*eStep[j]);
				//if ( std::isnan(eY[j][i]) == 1 ) { cout << "NAN " << i << " / " << 3*eRadius[j] << ", " << eX[j][i] << ", " << eY[j][i] << endl; }
			}
			else {
				eX[j][i] = eA[j]+eStep[j];
				eY[j][i] = eY[j][i-1];
			}
		
		}

		for (int i=3*eRadius[j]; i<4*eRadius[j]; i++) {
			eStep[j] += 1;
			
			if (eRadius[j]*eRadius[j] >= eStep[j]*eStep[j]) {
				eX[j][i] = eA[j]+eStep[j];
				eY[j][i] = eB[j]+sqrt(eRadius[j]*eRadius[j]-eStep[j]*eStep[j]);
				//if ( std::isnan(eY[j][i]) == 1 ) { cout << "NAN " << i << " / " << 4*eRadius[j] << ", " << eX[j][i] << ", " << eY[j][i] << endl; }
			}
			else {
				eX[j][i] = eA[j]+eStep[j];
				eY[j][i] = eY[j][i-1];
			}
	
		}

		for (int i=0; i<4*eRadius[j]; i++) {
			if ( eX[j][i]*eX[j][i] + eY[j][i]*eY[j][i] <= eX[j][eDrawcursor[j]]*eX[j][eDrawcursor[j]] + eY[j][eDrawcursor[j]]*eY[j][eDrawcursor[j]] ) {
				eDrawcursor[j] = i;
			}

		}
		//cout << "eDrawcursor " << j+1 << ": " << eDrawcursor[j] << " / " << 4*eRadius[j] << endl;
		//cout << eX[j][eDrawcursor[j]] << ", " << eY[j][eDrawcursor[j]] << endl;
	
	}

	for (int l=0; l<nELECTRON; l++) {
		eX[l][int(4*eRadius[l])] = eX[l][int(4*eRadius[l])-1];
		eY[l][int(4*eRadius[l])] = eY[l][int(4*eRadius[l])-1];
	}


	// points
	for (int k=0; k<nELECTRON; k++) {

		ElectronX[k] = new Double_t[kNMAX];
		ElectronY[k] = new Double_t[kNMAX];
		ElectronZ[k] = new Double_t[kNMAX];

		ElectronX[k][0] = eX[k][eDrawcursor[k]];
		ElectronY[k][0] = eY[k][eDrawcursor[k]];
		ElectronZ[k][0] = 0;

	}


	for (int i=0; i<kNMAX-1; i++) {
		
		for (int j=0; j<nELECTRON; j++) {

			if ( ElectronX[j][i]*ElectronX[j][i] + ElectronY[j][i]*ElectronY[j][i] < ecalRadius*ecalRadius ) {	
				// electron
				if (Electron_Charge[j] == -1) {
					// px > 0, py > 0 ( A < 0, B > 0 )
					if ( eA[j] < 0 && eB[j] > 0 ) {
						if ( eDrawcursor[j] == 0 ) { eDrawcursor[j] = int(4*eRadius[j]); }
						eDrawcursor[j]--;
	 				}
	     				// px > 0, py < 0 ( A > 0, B > 0 )
			      		else if ( eA[j] > 0 && eB[j] > 0 ) {
						if ( eDrawcursor[j] == 0 ) { eDrawcursor[j] = int(4*eRadius[j]); }
						eDrawcursor[j]--;
			       		}
			     		// px < 0, py < 0 ( A > 0 , B < 0 )
			      		else if ( eA[j] > 0 && eB[j] < 0 ) {
						if ( eDrawcursor[j] == 0 ) { eDrawcursor[j] = int(4*eRadius[j]); }
						eDrawcursor[j]--;
			   		}
			   		// px < 0, py > 0 ( A < 0, B > 0 ) 
					else if ( eA[j] < 0 && eB[j] < 0 ) {
						if ( eDrawcursor[j] == 0 ) { eDrawcursor[j] = int(4*eRadius[j]); }
						eDrawcursor[j]--;
					}
				}

     	  			// positron
				// posttron: if X[j][eDrawcursor[j]+1] == Y[j][eDrawcursor[j]+1] == 0: eDrawcursor[j] = 0;
	    			else if (Electron_Charge[j] == 1) {
	      				// px > 0, py > 0 ( A > 0, B < 0 )
	      				if ( eA[j] > 0 && eB[j] < 0 ) {
						if (eX[j][eDrawcursor[j]+1] == 0 && eY[j][eDrawcursor[j]+1] == 0 ) { eDrawcursor[j] = 0; }
						eDrawcursor[j]++;
	      				}
		      			// px > 0, py < 0 ( A < 0, B < 0 )
		      			else if ( eA[j] < 0 && eB[j] < 0 ) {
						if (eX[j][eDrawcursor[j]+1] == 0 && eY[j][eDrawcursor[j]+1] == 0 ) { eDrawcursor[j] = 0; }
						eDrawcursor[j]++;
		     		 	}
		      			// px < 0, py < 0 ( A < 0 , B > 0 )
				      	else if ( eA[j] < 0 && eB[j] > 0 ) {
						if (eX[j][eDrawcursor[j]+1] == 0 && eY[j][eDrawcursor[j]+1] == 0 ) { eDrawcursor[j] = 0; }
						eDrawcursor[j]++;
				      	}
				     	// px < 0, py > 0 ( A > 0, B > 0 ) 
				      	else if ( eA[j] > 0 && eB[j] > 0 ) {
						if (eX[j][eDrawcursor[j]+1] == 0 && eY[j][eDrawcursor[j]+1] == 0 ) { eDrawcursor[j] = 0; }
						eDrawcursor[j]++;
		      			}

		    		}

				ElectronZ[j][i+1] = abs(1/Electron_Px[j])*Electron_Pz[j]*i;
			}
			else { ElectronZ[j][i+1] = ElectronZ[j][i]; }		

			ElectronX[j][i+1] = eX[j][eDrawcursor[j]];
			ElectronY[j][i+1] = eY[j][eDrawcursor[j]];

		}

	}

	
	for (int j=0; j<nPHOTON; j++) {
		ePhotonX[j] = new Double_t[kNMAX];
		ePhotonY[j] = new Double_t[kNMAX];
		ePhotonZ[j] = new Double_t[kNMAX];
	}
	
	for (int j=0; j<nPHOTON; j++) {
		//cout << Photon_Px[0] << endl;
		//scalingFactorPhoton[j] = std::max(abs(Photon_Px[j]), std::max(abs(Photon_Py[j]), abs(Photon_Pz[j])));
		scalingFactorPhoton[j] = 1/Electron_Px[j];
		//cout << scalingFactorPhoton[j] << " IS THE BIGGEST ONE " << endl;
		for (int i=0; i<kNMAX; i++) {
			ePhotonX[j][i] = (i+1)*Photon_Px[j]*scalingFactorPhoton[j];
			ePhotonY[j][i] = (i+1)*Photon_Py[j]*scalingFactorPhoton[j];
			ePhotonZ[j][i] = (i+1)*Photon_Pz[j]*scalingFactorPhoton[j];
		}
		//cout << ePhotonX[0][0] << ", " << ePhotonY[0][0] << ", " << ePhotonZ[0][0] << endl;
	} 

}


int j=0;


void AnimateMuon() {

	if ( proton_bunch1->GetZ() != 0 && proton_bunch2->GetZ() != 0 ) {
		proton_bunch1->SetPosition(0,0,2000-10*proton_step);
		proton_bunch2->SetPosition(0,0,-2000+10*proton_step);
		gPad->Modified();	
		gPad->Update();
		proton_step += 1;
//		cout << proton_step << endl;
	}

	else { 	

		if ( draw_protons->IsDown() == true ) {
			for (int j=0; j<nPROTON; j++) {
				if ( proton_px[j][t]*proton_px[j][t] + proton_py[j][t]*proton_py[j][t] <= hcalRadius*hcalRadius ) {
					protons[j]->AddPoint(proton_px[j][t], proton_py[j][t], proton_pz[j][t], t);
					protons[j]->SetLineColor(kYellow);
//				cout << "PROTON t " << t << endl;
//				cout << "MUON " << MuonX[0][t] << endl;
					protons[j]->Draw();
				}
				else { break; }
			}
		}

		if ( draw_muons->IsDown() == true ) {
			if (MuonX[0][t]*MuonX[0][t] + MuonY[0][t]*MuonY[0][t] <= muonChamber*muonChamber || MuonX[1][t]*MuonX[1][t] + MuonY[1][t]*MuonY[1][t] <= muonChamber*muonChamber ) {

				for (int i=0; i<nMUON; i++) {
					muonTrack[i]->AddPoint(MuonX[i][t], MuonY[i][t], MuonZ[i][t], t);
					muonTrack[i]->SetLineColor(kBlue);
					muonTrack[i]->SetLineWidth(3);
					muonTrack[i]->Draw("SAME");
				}
			}

			else { muTimer->TurnOff(); cout << "muTIMER OFF" << endl; }
		}
	
		if ( draw_photons->IsDown() == true) {
			for (int j=0; j<nPHOTON; j++) {
				if ( muPhotonX[j][t]*muPhotonX[j][t] + muPhotonY[j][t]*muPhotonY[j][t] <= ecalRadius*ecalRadius ) {
					gammaTrack[j]->AddPoint(muPhotonX[j][t], muPhotonY[j][t], muPhotonZ[j][t], t);
					gammaTrack[j]->SetLineColor(kRed);
					gammaTrack[j]->SetLineWidth(3);
					gammaTrack[j]->Draw("SAME");
//					cout << "DRAW PHOTON " << endl;			
				}
			}
		}

		t += 2;
	}
//		cout << "MUON t " << t << endl;
}
 


void AnimateElectron() {


	if ( proton_bunch1->GetZ() != 0 && proton_bunch2->GetZ() != 0 ) {
		proton_bunch1->SetPosition(0,0,2000-10*proton_step);
		proton_bunch2->SetPosition(0,0,-2000+10*proton_step);
		gPad->Modified();	
		gPad->Update();
		proton_step += 1;
//		cout << proton_step << endl;
	}

	else {

		if ( draw_protons->IsDown() == true ) {
			for (int j=0; j<nPROTON; j++) {
				if ( proton_px[j][t]*proton_px[j][t] + proton_py[j][t]*proton_py[j][t] <= hcalRadius*hcalRadius ) {
					protons[j]->AddPoint(proton_px[j][t], proton_py[j][t], proton_pz[j][t], t);
					protons[j]->SetLineColor(kYellow);
		//			cout << "PROTON t " << t << endl;
	//				cout << "MUON " << MuonX[0][t] << endl;
					protons[j]->Draw();
				}
			//else { break; }
				else { eTimer->TurnOff(); cout << "eTIMER OFF" << endl;}
			}
		}

		if ( draw_electrons->IsDown() == true ) {
			if ( ElectronX[0][t]*ElectronX[0][t] + ElectronY[0][t]*ElectronY[0][t] <= ecalRadius*ecalRadius || ElectronX[1][t]*ElectronX[1][t] + ElectronY[1][t]*ElectronY[1][t] <= ecalRadius*ecalRadius ) {
			
				for (int i=0; i<nELECTRON; i++) {	
					electronTrack[i]->AddPoint(ElectronX[i][t], ElectronY[i][t], ElectronZ[i][t], t);
					electronTrack[i]->SetLineColor(kCyan);
					electronTrack[i]->SetLineWidth(3);
					electronTrack[i]->Draw("SAME");
				}
			}
		}		
		
		if ( draw_photons->IsDown() == true ) {
			for (int j=0; j<nPHOTON; j++) {
				if ( ePhotonX[j][t]*ePhotonX[j][t] + ePhotonY[j][t]*ePhotonY[j][t] <= ecalRadius*ecalRadius ) {
					gammaTrack[j]->AddPoint(ePhotonX[j][t], ePhotonY[j][t], ePhotonZ[j][t], t);
					gammaTrack[j]->SetLineColor(kRed);
					gammaTrack[j]->SetLineWidth(3);
					gammaTrack[j]->Draw("SAME");
				//	cout << "DRAW PHOTON " << endl;			
				}
			}
		}

		t += 2;
	}
		//else { eTimer->TurnOff(); cout << "eTIMER OFF" << endl;}

} 


MyMainFrame::~MyMainFrame() {
	// Clean up used widgets: frames, buttons, layout hints
	fMain->Cleanup();
   	delete fMain;
}

