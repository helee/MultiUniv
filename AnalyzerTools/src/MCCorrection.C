#include "MCCorrection.h"
#include "RootHelper.h"

MCCorrection::MCCorrection() : 
IgnoreNoHist(false)
{

}

void MCCorrection::ReadHistograms(){

  TString datapath = getenv("DATA_DIR");


  //==== ID/Trigger
  TString IDpath = datapath+"/"+TString::Itoa(DataYear,10)+"/ID/";

  string elline;
  ifstream in(IDpath+"/Electron/histmap.txt");
  while(getline(in,elline)){
    std::istringstream is( elline );

    TString tstring_elline = elline;
    if(tstring_elline.Contains("#")) continue;

    TString a,b,c,d,e,f;
    is >> a; // ID,RERCO
    is >> b; // Eff,SF
    is >> c; // <WPnames>
    is >> d; // <rootfilename>
    is >> e; // <histname>
    is >> f; // Class
    TFile *file = new TFile(IDpath+"/Electron/"+d);

    if(f=="TH2F"){
      map_hist_Electron[a+"_"+b+"_"+c] = (TH2F *)file->Get(e);
      cout<<"map_hist_Electron["<<a+"_"+b+"_"+c<<"]"<<" is for file "<<e<<endl;
    }
    else if(f=="TGraphAsymmErrors"){
      map_graph_Electron[a+"_"+b+"_"+c] = (TGraphAsymmErrors *)file->Get(e);
    }
    else{
      cout << "[MCCorrection::MCCorrection] Wrong class type : " << elline << endl;
    }
  }

  cout << "[MCCorrection::MCCorrection] map_hist_Electron :" << endl;
  for(std::map< TString, TH2F* >::iterator it=map_hist_Electron.begin(); it!=map_hist_Electron.end(); it++){
    cout << it->first << endl;
  }
  cout << "[MCCorrection::MCCorrection] map_graph_Electron :" << endl;
  for(std::map< TString, TGraphAsymmErrors* >::iterator it=map_graph_Electron.begin(); it!=map_graph_Electron.end(); it++){
    cout << it->first << endl;
  }


  string elline2;
  ifstream in2(IDpath+"/Muon/histmap.txt");
  while(getline(in2,elline2)){
    std::istringstream is( elline2 );

    TString tstring_elline = elline2;
    if(tstring_elline.Contains("#")) continue;

    TString a,b,c,d,e;
    is >> a; // ID,RERCO
    is >> b; // Eff,SF
    is >> c; // <WPnames>
    is >> d; // <rootfilename>
    is >> e; // <histname>
    TFile *file = new TFile(IDpath+"/Muon/"+d);
    map_hist_Muon[a+"_"+b+"_"+c] = (TH2F *)file->Get(e);
  }

  cout << "[MCCorrection::MCCorrection] map_hist_Muon :" << endl;
  for(std::map< TString, TH2F* >::iterator it=map_hist_Muon.begin(); it!=map_hist_Muon.end(); it++){
    cout << it->first << endl;
  }


  // == Get Prefiring maps
  TString PrefirePath  = datapath+"/"+TString::Itoa(DataYear,10)+"/Prefire/";

  string elline3;
  ifstream in3(PrefirePath+"/histmap.txt");
  while(getline(in3,elline3)){
    std::istringstream is( elline3 );

    TString tstring_elline = elline3;
    if(tstring_elline.Contains("#")) continue;

    TString a,b,c;
    is >> a; // Jet, Photon
    is >> b; // <rootfilename>
    is >> c; // <histname>
    
    TFile *file = new TFile(PrefirePath+b);
    map_hist_prefire[a + "_prefire"] = (TH2F *)file->Get(c);
  }


  // == Get Pileup Reweight maps
  map_hist_pileup.clear();
  TString PUReweightPath = datapath+"/"+TString::Itoa(DataYear,10)+"/PileUp/";

  string elline4;
  ifstream  in4(PUReweightPath+"/histmap.txt");
  while(getline(in4,elline4)){
    std::istringstream is( elline4 );

    TString tstring_elline = elline4;
    if(tstring_elline.Contains("#")) continue;

    TString a,b,c;
    is >> a; // sample name
    is >> b; // syst
    is >> c; // rootfile name

    if(DataYear == 2017 && a!=MCSample) continue;
    
    TFile *file = new TFile(PUReweightPath+c);
    cout<<"MCCorrection:: getting PU hist: "<<a+"_"+b<<endl;
    if((TH1D *)file->Get(a+"_"+b)) map_hist_pileup[a+"_"+b+"_pileup"] = (TH1D *)file->Get(a+"_"+b);
    else{
      cout << "[MCCorrection::ReadHistograms] No : " << a + "_" + b << endl;
    }
  }
  if(DataYear == 2017 && map_hist_pileup.size() == 0){
    cout << "[MCCorrection::ReadHistograms] Pileup: using 2017 default profile DYJets" <<endl;
    in4.clear();
    in4.seekg(0, ios::beg);
    while(getline(in4,elline4)){
      std::istringstream is( elline4 );

      TString tstring_elline = elline4;
      if(tstring_elline.Contains("#")) continue;

      TString a,b,c;
      is >> a; // sample name
      is >> b; // syst
      is >> c; // rootfile name

      if(DataYear == 2017 && a!="DYJets") continue;
      
      TFile *file = new TFile(PUReweightPath+c);
      cout<<"MCCorrection:: getting PU hist: "<<a+"_"+b<<endl;
      if((TH1D *)file->Get(a+"_"+b)) map_hist_pileup[a+"_"+b+"_pileup"] = (TH1D *)file->Get(a+"_"+b);
      else{
        cout << "[MCCorrection::ReadHistograms] No : " << a + "_" + b << endl;
      }
    }
  }
  //=====================================
  // ZpT weight maps
  //=====================================
  map_hist_ZpT.clear();
  TString ZpTweightPath = datapath+"/"+TString::Itoa(DataYear,10)+"/ZpT/";

  string zptInline;
  ifstream f_zpt(ZpTweightPath + "histmap.txt");
  if(!f_zpt.is_open()){
    cout << "[MCCorrection:ZpT] no file "<<ZpTweightPath + "histmap.txt"  <<endl;
    exit(EXIT_FAILURE);
  }
  while(getline(f_zpt, zptInline)){
    std::istringstream is( zptInline );

    TString tstring_elline = zptInline;
    if(tstring_elline.Contains("#")) continue;

    TString a,b,c,d,e;
    is >> a; // sample name
    is >> b; // flavor
    is >> c; // number of iteration
    is >> d; // rootfile name
    is >> e; // hist name
    //cout<<a<<"\t"<<b<<"\t"<<c<<"\t"<<d<<"\t"<<e<<endl;
    TFile *fzpt = new TFile(ZpTweightPath + d);
    //TString sflavour[2]={"muon","electron"};
    //TH2D **hzpt=NULL,**hzpt_norm=NULL;
    //for(int ifl=0;ifl<2;ifl++){
    //if(ifl==0){
    //  hzpt=&hzpt_muon;
    //  hzpt_norm=&hzpt_norm_muon;
    //}else if(ifl==1){
    //  hzpt=&hzpt_electron;
    //  hzpt_norm=&hzpt_norm_electron;
    //}
    cout<<"number of iteration for Zpt correction: "<<c.Atoi()<<endl;
      for(int i=0;i<c.Atoi();i++){
        TH2D* this_hzpt=(TH2D*)fzpt->Get(Form("%s%d",e.Data(),i));
        //TH2D* this_hzpt=(TH2D*)fzpt->Get(Form("%s%d_iter%d",b.Data(),DataYear,i));
	if(i==0)if(! this_hzpt){
	  cout<<"[MCCorrection::SetupZPtWeight] No ZpT correction histogram for "<<a<<" "<<b<<endl;
          exit(EXIT_FAILURE);
	}
        if(this_hzpt){
          if(map_hist_ZpT[a+"_"+b]){
            map_hist_ZpT[a+"_"+b]->Multiply(this_hzpt);
            //(*hzpt)->Multiply(this_hzpt);
            cout<<"[MCCorrection::SetupZPtWeight] setting "<<a<<" "<<b<<" zptcor iter"<<i<<endl;
          }else{
            map_hist_ZpT[a+"_"+b]=this_hzpt;
            cout<<"[MCCorrection::SetupZPtWeight] setting first "<<a<<" "<<b<<" zptcor"<<i<<endl;
          }
        }else break;
      }
      //if(*hzpt) (*hzpt)->SetDirectory(0);
      //*hzpt_norm=(TH2D*)fzpt.Get(Form("%s%d_norm",sflavour[ifl].Data(),DataYear));
      map_hist_ZpT[a+"_"+b+"_Norm"]=(TH2D*)fzpt->Get(Form("%s%d_norm",b.Data(),DataYear));
      if(map_hist_ZpT[a+"_"+b+"_Norm"]){
        //(*hzpt_norm)->SetDirectory(0);
        cout<<"[MCCorrection::SetupZPtWeight] setting "<<a<<" "<<b<<" zptcor norm"<<endl;
      }
    //}
  }

/*
  cout << "[MCCorrection::MCCorrection] map_hist_pileup :" << endl;
  for(std::map< TString, TH1D* >::iterator it=map_hist_pileup.begin(); it!=map_hist_pileup.end(); it++){
    cout << it->first << endl;
  }
*/

}

MCCorrection::~MCCorrection(){

}

void MCCorrection::SetMCSample(TString s){
  MCSample = s;
}
void MCCorrection::SetDataYear(int i){
  DataYear = i;
}

double MCCorrection::MuonID_SF(TString ID, double eta, double pt, int sys){

  if(ID=="Default") return 1.;

  //cout << "[MCCorrection::MuonID_SF] ID = " << ID << endl;
  //cout << "[MCCorrection::MuonID_SF] eta = " << eta << ", pt = " << pt << endl;

  double value = 1.;
  double error = 0.;

  if(DataYear==2017){
    eta = fabs(eta);
  }

  if(ID=="NUM_TightID_DEN_genTracks" || ID=="NUM_HighPtID_DEN_genTracks"){
    //==== boundaries
    if(pt<20.) pt = 20.;
    if(pt>=120.) pt = 119.;
    if(eta>=2.4) eta = 2.39;
    if(eta<-2.4) eta = -2.4;
  }

  TH2F *this_hist = map_hist_Muon["ID_SF_"+ID];
  if(!this_hist){
    if(IgnoreNoHist) return 1.;
    else{
      cout << "[MCCorrection::MuonID_SF] No "<<"ID_SF_"+ID<<endl;
      exit(EXIT_FAILURE);
    }
  }

  int this_bin(-999);

  if(DataYear==2016){
    this_bin = this_hist->FindBin(eta,pt);
  }
  else if(DataYear==2017){
    this_bin = this_hist->FindBin(pt,eta);
  }
  else{
    cout << "[MCCorrection::MuonID_SF] Wrong year : "<<DataYear<<endl;
    exit(EXIT_FAILURE);
  }

  value = this_hist->GetBinContent(this_bin);
  error = this_hist->GetBinError(this_bin);

  //cout << "[MCCorrection::MuonID_SF] value = " << value << endl;

  return value+double(sys)*error;

}

double MCCorrection::MuonISO_SF(TString ID, double eta, double pt, int sys){

  if(ID=="Default") return 1.;

  //cout << "[MCCorrection::MuonISO_SF] eta = " << eta << ", pt = " << pt << endl;

  double value = 1.;
  double error = 0.;

  if(DataYear==2017){
    eta = fabs(eta);
  }

  if(ID=="NUM_TightRelIso_DEN_TightIDandIPCut" || ID=="NUM_LooseRelTkIso_DEN_HighPtIDandIPCut"){
    //==== boundaries
    if(pt<20.) pt = 20.;
    if(pt>=120.) pt = 119.;
    if(eta>=2.4) eta = 2.39;
    if(eta<-2.4) eta = -2.4;
  }

  TH2F *this_hist = map_hist_Muon["ISO_SF_"+ID];
  if(!this_hist){
    if(IgnoreNoHist) return 1.;
    else{
      cout << "[MCCorrection::MuonISO_SF] No "<<"ISO_SF_"+ID<<endl;
      exit(EXIT_FAILURE);
    }
  }

  int this_bin(-999);

  if(DataYear==2016){
    this_bin = this_hist->FindBin(eta,pt);
  }
  else if(DataYear==2017){
    this_bin = this_hist->FindBin(pt,eta);
  }
  else{
    cout << "[MCCorrection::MuonISO_SF] Wrong year : "<<DataYear<<endl;
    exit(EXIT_FAILURE);
  }

  value = this_hist->GetBinContent(this_bin);
  error = this_hist->GetBinError(this_bin);

  //cout << "[MCCorrection::MuonISO_SF] value = " << value << endl;

  return value+double(sys)*error;

}

double MCCorrection::MuonTrigger_Eff(TString ID, TString trig, int DataOrMC, double eta, double pt, int sys){

  if(ID=="Default") return 1.;

  //cout << "[MCCorrection::MuonTrigger_Eff] ID = " << ID << "\t" << "trig = " << trig << endl;
  //cout << "[MCCorrection::MuonTrigger_Eff] DataOrMC = " << DataOrMC << endl;
  //cout << "[MCCorrection::MuonTrigger_Eff] eta = " << eta << ", pt = " << pt << endl;

  double value = 1.;
  double error = 0.;

  //FIXME no 2016 trigger SF YET. Should check this layer
  if(DataYear==2017){
    eta = fabs(eta);
  }

  //==== 2016
  if(DataYear==2016){
    if(trig=="IsoMu24"){
      if(pt<26.) return 1.; //FIXME
      if(eta>=2.4) eta = 2.39;
      if(eta<-2.4) eta = -2.4;

      if(pt>1200.) pt = 1199.;
    }
    else if(trig=="Mu50"){
      if(pt<52.) return 1.; //FIXME
      if(eta>=2.4) eta = 2.39;

      if(pt>1200.) pt = 1199.;
    }
    else{

    }
  }
  else if(DataYear==2017){
    if(trig=="IsoMu27"){
      //==== FIXME MiniAODPt Pt
      //==== FIXME 28.9918  29.0363
      //==== FIXME This event pass pt>29GeV cut, but MiniAOD pt < 29 GeV
      //==== FIXME So when I return 0., SF goes nan.. let's returning 1. for now..
      if(pt<29.) return 1.; //FIXME
      if(eta>=2.4) eta = 2.39;

      if(pt>1200.) pt = 1199.;
    }
    else if(trig=="Mu50"){
      if(pt<52.) return 1.; //FIXME
      if(eta>=2.4) eta = 2.39;

      if(pt>1200.) pt = 1199.;
    }
    else{

    }
  }

  TString histkey = "Trigger_Eff_DATA_"+trig+"_"+ID;
  if(DataOrMC==1) histkey = "Trigger_Eff_MC_"+trig+"_"+ID;
  //cout << "[MCCorrection::MuonTrigger_Eff] histkey = " << histkey << endl;
  TH2F *this_hist = map_hist_Muon[histkey];
  if(!this_hist){
    if(IgnoreNoHist) return 1.;
    else{
      cout << "[MCCorrection::MuonTrigger_Eff] No "<<histkey<<endl;
      exit(EXIT_FAILURE);
    }
  }

  int this_bin(-999);

  if(DataYear==2016){
    //FIXME no 2016 trigger SF YET. Should check this layer
    this_bin = this_hist->FindBin(eta,pt);
  }
  else if(DataYear==2017){
    this_bin = this_hist->FindBin(pt,eta);
  }
  else{
    cout << "[MCCorrection::MuonTrigger_Eff] Wrong year : "<<DataYear<<endl;
    exit(EXIT_FAILURE);
  }

  value = this_hist->GetBinContent(this_bin);
  error = this_hist->GetBinError(this_bin);

  //cout << "[MCCorrection::MuonTrigger_Eff] value = " << value << endl;

  return value+double(sys)*error;


}

double MCCorrection::MuonTrigger_SF(TString ID, TString trig, std::vector<Muon> muons, int sys){

  if(ID=="Default") return 1.;

  double value = 1.;

  if(trig=="IsoMu24" || trig=="IsoMu27" || trig=="Mu50"){

    double eff_DATA = 1.;
    double eff_MC = 1.;

    for(unsigned int i=0; i<muons.size(); i++){
      eff_DATA *= ( 1.-MuonTrigger_Eff(ID, trig, 0, muons.at(i).Eta(), muons.at(i).MiniAODPt(), sys) );
      eff_MC   *= ( 1.-MuonTrigger_Eff(ID, trig, 1, muons.at(i).Eta(), muons.at(i).MiniAODPt(), -sys) );
    }

    eff_DATA = 1.-eff_DATA;
    eff_MC = 1.-eff_MC;

    value = eff_DATA/eff_MC;

/*
    if(eff_DATA==0||eff_MC==0){
      cout << "==== Zero Trigger Eff ====" << endl;
      for(unsigned int i=0;i<muons.size();i++){
        cout << muons.at(i).MiniAODPt() << "\t" << muons.at(i).Pt() << endl;
      }
    }
*/

  }

  return value;

}

double MCCorrection::ElectronID_SF(TString ID, double sceta, double pt, int sys){

  if(ID=="Default") return 1.;

  double value = 1.;
  double error = 0.;

  if(pt<10.) pt = 10.;
  if(pt>=500.) pt = 499.;
  if(sceta>=2.5) sceta = 2.49;
  if(sceta<-2.5) sceta = -2.5;

  if( ID.Contains("HEEP") ){

    TString this_key = "ID_SF_"+ID;
    if(fabs(sceta) < 1.479) this_key += "_Barrel";
    else                    this_key += "_Endcap";

    TGraphAsymmErrors *this_graph = map_graph_Electron[this_key];
    if(!this_graph){
      if(IgnoreNoHist) return 1.;
      else{
        cout << "[MCCorrection::ElectronID_SF] (Graph) No "<<this_key<<endl;
        exit(EXIT_FAILURE);
      }
    }

    int NX = this_graph->GetN();

    for(int j=0; j<NX; j++){

      double x, x_low, x_high;
      double y, y_low, y_high;
      this_graph->GetPoint(j, x, y);
      x_low = this_graph->GetErrorXlow(j);
      x_high = this_graph->GetErrorXhigh(j);

      if(j==0 && pt < x-x_low ) pt = x-x_low;
      if(j==NX-1 && x+x_high <= pt ) pt = x-x_low;

      if( x-x_low <= pt && pt < x+x_high){
        y_low = this_graph->GetErrorYlow(j);
        y_high = this_graph->GetErrorYhigh(j);

        if(sys==0) return y;
        else if(sys>0) return y+y_high;
        else return y-y_low;

      }

    }
    cout << "[MCCorrection::ElectronID_SF] (Graph) pt range strange.. "<<"ID_SF_"+ID<<", with pt = " << pt << endl;
    exit(EXIT_FAILURE);
    return 1.;

  }
  else{

    TH2F *this_hist = map_hist_Electron["ID_SF_"+ID];
    if(!this_hist){
      if(IgnoreNoHist) return 1.;
      else{
        cout << "[MCCorrection::ElectronID_SF] (Hist) No "<<"ID_SF_"+ID<<endl;
        exit(EXIT_FAILURE);
      }
    }

    int this_bin = this_hist->FindBin(sceta,pt);
    value = this_hist->GetBinContent(this_bin);
    error = this_hist->GetBinError(this_bin);

    return value+double(sys)*error;

  }

}

double MCCorrection::ElectronReco_SF(double sceta, double pt, int sys){

  double value = 1.;
  double error = 0.;

  TString ptrange = "ptgt20";
  if(pt<20.) ptrange = "ptlt20";

  if(pt<10.) pt = 10.;
  if(pt>=500.) pt = 499.;
  if(sceta>=2.5) sceta = 2.49;
  if(sceta<-2.5) sceta = -2.5;

  TH2F *this_hist = map_hist_Electron["RECO_SF_"+ptrange];
  if(!this_hist){
    if(IgnoreNoHist) return 1.;
    else{
      cout << "[MCCorrection::ElectronReco_SF] No "<<"RECO_SF_"+ptrange<<endl;
      exit(EXIT_FAILURE);
    }
  }

  //cout << "[MCCorrection::ElectronReco_SF] " << this_hist->GetBinContent(1,1) << endl;

  int this_bin = this_hist->FindBin(sceta,pt);
  value = this_hist->GetBinContent(this_bin);
  error = this_hist->GetBinError(this_bin);

  return value+double(sys)*error;

}


double MCCorrection::GetPrefireWeight(std::vector<Photon> photons, std::vector<Jet> jets, int sys){

  double photon_weight = 1.;
  double jet_weight = 1.;
  
  TH2F *photon_hist = map_hist_prefire["Photon_prefire"];
  TH2F *jet_hist = map_hist_prefire["Jet_prefire"];

  
  for(unsigned int i_pho = 0; i_pho < photons.size(); i_pho++){
    Photon current_photon = photons.at(i_pho);
    double eta = current_photon.scEta();
    double pt = current_photon.Pt();
    
    int this_bin = photon_hist->FindBin(eta, pt);

    double this_eff = photon_hist->GetBinContent(this_bin);
    double this_efferr = photon_hist->GetBinError(this_bin);

    double current_weight = 1. - (this_eff + (double)sys * this_efferr );;
    photon_weight = photon_weight * current_weight;
  }
  
  for(unsigned int i_jet = 0; i_jet < jets.size(); i_jet++){
    Jet current_jet = jets.at(i_jet);
    double eta = current_jet.Eta();
    double pt = current_jet.Pt();
    
    int this_bin = jet_hist->FindBin(eta, pt);

    double this_eff = jet_hist->GetBinContent(this_bin);
    double this_efferr = jet_hist->GetBinError(this_bin);

    double current_weight = 1.- (this_eff + (double)sys * this_efferr);
    jet_weight = jet_weight * current_weight;
  }

  return photon_weight * jet_weight;;

}


double MCCorrection::GetPileUpWeightBySampleName(int N_vtx, int syst){
  
  int this_bin = N_vtx+1;
  if(N_vtx >= 100) this_bin=100;

  TString this_histname = MCSample;
  if(syst == 0){
    this_histname += "_central_pileup";
  }
  else if(syst == -1){
    this_histname += "_sig_down_pileup";
  }
  else if(syst == 1){
    this_histname += "_sig_up_pileup";
  }
  else{
    cout << "[MCCorrection::GetPileUpWeightBySampleName] syst should be 0, -1, or +1" << endl;
    exit(EXIT_FAILURE);
  }

  TH1D *this_hist = map_hist_pileup[this_histname];
  if(!this_hist){
    if(DataYear == 2017){
      this_histname = "DYJets";// Default for 2017
      if(syst == 0){
        this_histname += "_central_pileup";
      }
      else if(syst == -1){
        this_histname += "_sig_down_pileup";
      }
      else if(syst == 1){
        this_histname += "_sig_up_pileup";
      }
      else{
        cout << "[MCCorrection::GetPileUpWeightBySampleName] syst should be 0, -1, or +1" << endl;
        exit(EXIT_FAILURE);
      }

      this_hist = map_hist_pileup[this_histname];
      if(!this_hist){
	cout<<"[MCCorrection::GetPileUpWeightBySampleName] No " << this_histname << endl;
	exit(EXIT_FAILURE);
      }
    }
    else{
      cout << "[MCCorrection::GetPileUpWeightBySampleName] No " << this_histname << endl;
      exit(EXIT_FAILURE);
    }
  }

  return this_hist->GetBinContent(this_bin);

}

double MCCorrection::GetPileUpWeight(int N_vtx, int syst){

  int this_bin = N_vtx+1;
  if(N_vtx >= 100) this_bin=100;

  TString this_histname = "MC_" + TString::Itoa(DataYear,10);
  if(syst == 0){
    this_histname += "_central_pileup";
  }
  else if(syst == -1){
    this_histname += "_sig_down_pileup";
  }
  else if(syst == 1){
    this_histname += "_sig_up_pileup";
  }
  else{
    cout << "[MCCorrection::GetPileUpWeight] syst should be 0, -1, or +1" << endl;
    exit(EXIT_FAILURE);
  }

  TH1D *this_hist = map_hist_pileup[this_histname];
  if(!this_hist){
    cout << "[MCCorrection::GetPileUpWeight] No " << this_histname << endl;
    exit(EXIT_FAILURE);
  }

  return this_hist->GetBinContent(this_bin);

}
double MCCorrection::DiLeptonTrg_SF(TString IdKey0,TString IdKey1,const vector<Lepton*>& leps,int sys){
  if(leps.size() < 2){
    cout<<"[MCCorrection::DiLeptonTrg_SF] only dilepton algorithm"<<endl;
    return 1;
  }
  //cout<<"DiLeptonTrg: pt0 "<<leps[0]->Pt()<<endl;
  std::map< TString, TH2F* > map_hist_Lepton;
  if(leps[0]->LeptonFlavour()==Lepton::MUON){
    map_hist_Lepton = map_hist_Muon;
  }else if(leps[0]->LeptonFlavour()==Lepton::ELECTRON){
    map_hist_Lepton = map_hist_Electron;
  }else{
    cout <<"[MCCorrection::DiLeptonTrg_SF] Only for Muon or Electron"<<endl;
    exit(EXIT_FAILURE);
  }    
      
  double this_pt[2]={},this_eta[2]={};
  for(int i=0;i<2;i++){
    if(leps[i]->LeptonFlavour()==Lepton::MUON){
      this_pt[i]=((Muon*)leps.at(i))->MiniAODPt();
      this_eta[i]=leps.at(i)->Eta();
    }else if(leps[i]->LeptonFlavour()==Lepton::ELECTRON){
      this_pt[i]=leps.at(i)->Pt();
      this_eta[i]=((Electron*)leps.at(i))->scEta();
    }else{
      cout << "[MCCorrection::DiLeptonTrg_SF] It is not lepton"<<endl;
      exit(EXIT_FAILURE);
    }
  }
  if(DataYear==2016&&leps[0]->LeptonFlavour()==Lepton::MUON){
    TH2F* this_hist[8]={};
    TString sdata[2]={"DATA","MC"};
    TString speriod[2]={"BCDEF","GH"};
    for(int id=0;id<2;id++){
      for(int ip=0;ip<2;ip++){
	this_hist[4*id+2*ip]=map_hist_Lepton["Trigger_Eff_"+sdata[id]+"_"+IdKey0+"_"+speriod[ip]];
	this_hist[4*id+2*ip+1]=map_hist_Lepton["Trigger_Eff_"+sdata[id]+"_"+IdKey1+"_"+speriod[ip]];
      }
    }
    if(!this_hist[0]||!this_hist[1]||!this_hist[2]||!this_hist[3]){
      cout << "[SMPValidation::Trigger_SF] No "<<IdKey0<<" or "<<IdKey1<<endl;
      exit(EXIT_FAILURE);
    }

    double lumi_periodB = 5.929001722;
    double lumi_periodC = 2.645968083;
    double lumi_periodD = 4.35344881;
    double lumi_periodE = 4.049732039;
    double lumi_periodF = 3.157020934;
    double lumi_periodG = 7.549615806;
    double lumi_periodH = 8.545039549 + 0.216782873;

    double total_lumi = (lumi_periodB+lumi_periodC+lumi_periodD+lumi_periodE+lumi_periodF+lumi_periodG+lumi_periodH);

    double WeightBtoF = (lumi_periodB+lumi_periodC+lumi_periodD+lumi_periodE+lumi_periodF)/total_lumi;
    double WeightGtoH = (lumi_periodG+lumi_periodH)/total_lumi;
    
    double triggerEff[4]={1.,1.,1.,1.};
    for(int i=0;i<4;i++){
      for(int il=0;il<2;il++){
	triggerEff[i]*=RootHelper::GetBinContentUser(this_hist[2*i+il],this_eta[il],this_pt[il],(i<2?1.:-1.)*sys);
      }
    }
    return (triggerEff[0]*WeightBtoF+triggerEff[1]*WeightGtoH)/(triggerEff[2]*WeightBtoF+triggerEff[3]*WeightGtoH);
  }else{
    TH2F* this_hist[2]={NULL,NULL};
    double triggerSF=1.;
    this_hist[0]=map_hist_Lepton["Trigger_SF_"+IdKey0];
    this_hist[1]=map_hist_Lepton["Trigger_SF_"+IdKey1];
    if(!this_hist[0]||!this_hist[1]){
      cout << "[MCCorrection::DiLepTrg_SF] No Trigger_SF_"<<IdKey0<<" or Trigger_SF_"<<IdKey1<<endl;
      exit(EXIT_FAILURE);
    }
    for(int i=0;i<2;i++){
      triggerSF*=RootHelper::GetBinContentUser(this_hist[i],this_eta[i],this_pt[i],sys);
    }
    return triggerSF;
  }   
}


double MCCorrection::GetZPtWeight(double zpt, double zrap, Lepton::Flavour flavour, TString sample){
  double valzptcor=1.;
  double valzptcor_norm=1.;
  TH2D* hzpt=NULL;
  TH2D* hzpt_norm=NULL;
  if(flavour==Lepton::MUON){
    hzpt      = map_hist_ZpT[sample+"_muon"];
    hzpt_norm = map_hist_ZpT[sample+"_muon_Norm"];
  }else if(flavour==Lepton::ELECTRON){
    hzpt      = map_hist_ZpT[sample+"_electron"];
    hzpt_norm = map_hist_ZpT[sample+"_electron_Norm"];
  }
  if(hzpt) valzptcor = RootHelper::GetBinContentUser(hzpt,zpt,zrap,0);
  if(hzpt_norm) valzptcor_norm = RootHelper::GetBinContentUser(hzpt_norm,zpt,zrap,0);
  return valzptcor*valzptcor_norm;
}








