// plot_bethebloch.C
// Macro per plottare la curva di Bethe-Bloch: dE/dx vs beta*gamma

void plot_bethebloch() {
    
    // ==========================
    // 1. APERTURA FILE E LETTURA
    // ==========================
    
    TFile* file = TFile::Open("output.root");
    if (!file || file->IsZombie()) {
        cout << "Errore: impossibile aprire output.root" << endl;
        return;
    }
    
    TTree* tree = (TTree*)file->Get("Simulation");
    if (!tree) {
        cout << "Errore: tree Simulation non trovato" << endl;
        return;
    }
    
    // Variabili per leggere i branch
    Double_t kineticEnergy, dEdx;
    tree->SetBranchAddress("KineticEnergy", &kineticEnergy);  // MeV
    tree->SetBranchAddress("dEdx", &dEdx);                   // MeV/mm
    
    cout << "Numero di entries: " << tree->GetEntries() << endl;
    
    
    // ==========================
    // 2. CALCOLO BETA*GAMMA
    // ==========================
    
    // Massa del protone in MeV/c²
    const Double_t mass_proton = 938.272;  // MeV/c²
    
    // Vettori per salvare i dati
    vector<Double_t> vec_betagamma;
    vector<Double_t> vec_dEdx;
    
    // Loop sugli eventi
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        
        // Salta valori non fisici
        if (kineticEnergy <= 0 || dEdx <= 0) continue;
        
        // Calcolo beta*gamma dalla energia cinetica
        // E_kin = (gamma - 1) * m * c²
        // gamma = 1 + E_kin/mc²
        Double_t gamma = 1.0 + kineticEnergy / mass_proton;
        
        // beta = sqrt(1 - 1/gamma²)
        Double_t beta = TMath::Sqrt(1.0 - 1.0/(gamma*gamma));
        
        // beta*gamma
        Double_t betagamma = beta * gamma;
        
        // Salva nei vettori
        vec_betagamma.push_back(betagamma);
        vec_dEdx.push_back(dEdx);
    }
    
    cout << "Punti validi: " << vec_betagamma.size() << endl;
    
    
    // ==========================
    // 3. CREAZIONE GRAFICO
    // ==========================
    
    TCanvas* c1 = new TCanvas("c1", "Bethe-Bloch Curve", 1000, 700);
    //c1->SetLogx();  // Scala logaritmica in x
    c1->SetGrid();
    
    // Crea il grafico scatter
    TGraph* gr = new TGraph(vec_betagamma.size(), 
                            vec_betagamma.data(), 
                            vec_dEdx.data());
    
    // Estetica
    gr->SetTitle("Bethe-Bloch Formula;#beta#gamma;dE/dx [MeV/mm]");
    gr->SetMarkerStyle(20);      // Cerchietti pieni
    gr->SetMarkerSize(0.3);      // Piccoli
    gr->SetMarkerColor(kBlue);
    
    // Disegna
    gr->Draw("AP");  // A = assi, P = points
    
    // Aggiungi griglia
    gPad->SetGrid();
    
    
    // ==========================
    // 4. SALVATAGGIO
    // ==========================
    
    c1->SaveAs("bethebloch_betagamma.png");
    c1->SaveAs("bethebloch_betagamma.pdf");
    
    cout << "\nGrafico salvato come bethebloch_betagamma.png/pdf" << endl;
    cout << "Range beta*gamma: [" << TMath::MinElement(vec_betagamma.size(), vec_betagamma.data()) 
         << ", " << TMath::MaxElement(vec_betagamma.size(), vec_betagamma.data()) << "]" << endl;
}
