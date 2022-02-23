// ROOT code for reading in the ASCII text output from logger.py.
// The code calculates and plots the waiting time, and histograms the count distribution.

{

	TTree *tree=new TTree();
//	tree->ReadFile("bg_8hrs.txt","time:adc");
	tree->ReadFile("aregcode_micro.txt","time:adc"); //Update the filename. Get rid of ":adc" if you're reading the data from logger_barebone.py
	TH1F *tw=new TH1F("tw","Waiting time between consecutive hits",16000,-0.,0.008); 
	tw->GetXaxis()->SetTitle("Waiting time [seconds]");
	double last_time=0;

	//calculate the waiting time
	for(int i=1;i<=tree->GetEntries();++i){
		tree->GetEntry(i); 
		double current_time=tree->GetBranch("time")->GetLeaf("time")->GetValue();
		tw->Fill((current_time-last_time)*1e-6);
		last_time=current_time;
	}

	TCanvas *c=new TCanvas();
	c->Divide(2,1);
	c->cd(1);
	tw->Draw();

	c->cd(2);
	float total_time=tree->GetMaximum("time");
	float time_window=1; //the time window by which we'll split the data, in seconds
	int time_windows = (int)total_time*1e-6/time_window;
	int nevents=tree->GetEntries();
	std::cout << "Time windows, entries:\t" << time_windows << "\t" << nevents << std::endl;
	TH1F *probability=new TH1F("probability",Form("Probability distributions of the counts. Time window=%5.1f sec",time_window),100,0,100);
	probability->GetXaxis()->SetTitle("Counts");
	probability->GetYaxis()->SetTitle("Windows");

	for(int t=0;t<time_windows;++t){
		int counts;
		counts=0;
		for(int j=1;j<=nevents;++j){
			tree->GetEntry(j);
			float time = tree->GetBranch("time")->GetLeaf("time")->GetValue();
			if(time >= t*time_window && time< (t+1)*time_window) ++counts;
		}
		probability->Fill(counts);
//		std::cout << counts <<", ";

	}
	probability->Draw();

	//now, plot the probability distribution

}