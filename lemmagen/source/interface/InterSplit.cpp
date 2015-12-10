/******************************************************************************
This file is part of the lemmagen library. It gives support for lemmatization.
Copyright (C) 2006-2007 Matjaz Jursic <matjaz@gmail.com>

The lemmagen library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/
#pragma once

#include "../header/WordList.h"
#include "Interface.h"

#define MSG_USAGE "\
                                                                              \n\
Splits example data file into smaller subsets for cross validation            \n\
Usage: lemSplit -k <split> [-t <type>] <infile> <outfilemask>                 \n\
                                                                              \n\
  -k, --split <split>    if type = 's','d': number of required subsets (cross)\n\
                         if type = 'p','r': percent of words to be in test set\n\
                         if type = 'c','e': number of required subsets (cross)\n\
  -t, --type <type>      specify type of split (default = deep), valid:       \n\
     s|shallow   (n sets)  split all sets according to example lines (cross v)\n\
     d|deep      (n sets)  split set according to example contents, all examp.\n\
                           with same word and lemma are in the same subset    \n\
                                                                              \n\
     p|perc      (2 sets)  split on 2 subsets using percent for test set      \n\
     r|percdeep  (2 sets)  combination of percent and deep functionality      \n\
                                                                              \n\
     c|cross     (2 sets)  split on 2 subsets using cross validation split    \n\
     e|crossdeep (2 sets)  combination of cross and deep functionality        \n\
                           r%k = set number, r/k = randomseed                 \n\
                                                                              \n\
  -r, --randseed <int>   seed for random number genarator (default=1), if     \n\
                           this switch is used with (<int>=0), time is used to\n\
                           generate seed, random is used for for spliting sets\n\
                                                                              \n\
  <infile>               input file (multext format)                          \n\
  <outfilemask>          output file mask ('#' replaced with sequence number) \n\
                                                                              \n\
  -v, --verbose          verbose messages                                     \n\
  -h, --help             print this help and exit                             \n\
      --version          print version information and exit                   \n\
                                                                              \n\
"

int InterSplit::EntryPoint(int argc, char **argv){
	CmdLineParser clParser;

	//parameters definition ------------------------------------------------------
	clParser.AddOption("split", optSwitch, optMandatory, "-k|--split");
	clParser.LastAdded().SetArgType(argInteger, 1, 1, "", "");
	clParser.AddOption("type", optSwitch, optOptional, "-t|--type");
	clParser.LastAdded().SetArgType(argDefined, 1, 1, "s|d|p|r|c|e|shallow|deep|perc|percdeep|cross|crossdeep", "deep");

	clParser.AddOption("randseed", optSwitch, optOptional, "-r|--randseed");
	clParser.LastAdded().SetArgType(argInteger, 1, 1, "", "");

	clParser.AddOption("infile", optDefault, optMandatory, "");
	clParser.LastAdded().SetArgType(argInFile, 1, 1, "", "");
	clParser.AddOption("outfilemask", optDefault, optMandatory, "");
	clParser.LastAdded().SetArgType(argOutFile, 1, 1, "", "");

	clParser.AddOption("verbose", optSwitch, optOptional, "-v|--verbose");
	clParser.AddOption("help", optSwitch, optOptional, "-h|--help");
	clParser.AddOption("version", optSwitch, optOptional, "--version");

	//parsing --------------------------------------------------------------------
	string sError = clParser.Parse(argc, argv);

	//errors & messages ----------------------------------------------------------
	if (clParser.GetOption("help").bSet) { cout << MSG_USAGE; return 0; }      //print usage
	if (clParser.GetOption("version").bSet) { cout << MSG_VERSION; return 0; } //print version
	if (sError != "") { cout << sError << "!\n" << MSG_HELP; return -1; }      //print error

	//ok arguments ok, read required options -------------------------------------
	string sInfile = clParser.GetOption("infile").vsArgVal[0];
	string sOutfileMask = clParser.GetOption("outfilemask").vsArgVal[0];
	int iSplit = clParser.GetOption("split").GetInteger();
	int iType = clParser.GetOption("type").GetAllowedIndex()%6;
	int iRandSeed  = abs(clParser.GetOption("randseed").GetInteger());	
	bool bVerbose = clParser.GetOption("verbose").bSet;

	//check for logical errors in parameters -------------------------------------
	if ( ((iType <2 || iType >=4) && (iSplit<2 || iSplit>500)) ||
		 ((iType >=2 && iType <4) && (iSplit<1 || iSplit>99)) ) {
		cout << "error in argument 'split':\n  if 'type'=s|d|c|e then allowed values are 2..500\n  if 'type'=p|r then allowed values are 1..99" << endl;
		return -1;
	}

	//output setting -------------------------------------------------------------
	cout << "chosen options:"  << endl;
	cout << "         input file: " << sInfile << endl;
	cout << "   output file mask: " << sOutfileMask << endl;
	cout << "      type of split: "; 
	if (iType == 0) cout << "shallow (" << iSplit << " sets)"; 
	if (iType == 1) cout << "deep (" << iSplit << " sets)";
	if (iType == 2) cout << "percent (" << iSplit << "% in test set)";
	if (iType == 3) cout << "percent deep (" << iSplit << "% in test set)";
	if (iType == 4) cout << "cross (1/" << iSplit << " in test set)";
	if (iType == 5) cout << "crossdeep (1/" << iSplit << " in test set)";
	cout << endl;
	cout << "        random seed: " << iRandSeed << endl;
	cout << "            verbose: " << (string)(bVerbose ? "yes" : "no") << endl;
	cout << "processing..."  << endl;


	//OK, finnaly do the magic ---------------------------------------------------
	if (Split(sInfile, sOutfileMask, iSplit, iType, iRandSeed, bVerbose) != 0) return -1;

	//report success -------------------------------------------------------------
	cout << "finished successfully"  << endl;
	return 0;
}


int InterSplit::Split(string sInfile, string sOutfileMask, int iSplit, int iType, int iRandSeed, bool bVerbose) {
	int iTakeSplit = 0;
	//randomization
	if (iType >=4 && iRandSeed>0) {
		iTakeSplit = (iRandSeed-1)%iSplit;
		iRandSeed = (iRandSeed-1)/iSplit + 1;
	}
	Rand::seed(iRandSeed);
	
	//timing
	Timing time;

	//import files	
	WordList wlst(sInfile.c_str(), NULL, bVerbose);
	if (iType<2 || iType>=4)
		wlst.SplitCrossValid(iSplit, iType, bVerbose*2);
	else
		wlst.SplitSubset(100-iSplit, iType%2, bVerbose*2);


	for (int iFile = 0; iFile < (iType<2 ? iSplit : 2); ++iFile) {
		stringstream ssFileNum;
		if (iType<2)
			ssFileNum << (iFile+1) << "of" << iSplit;
		else 
			ssFileNum << (iFile==0 ? "test" : "train");

		string sOutpuFile = sOutfileMask;
		int iFound = sOutfileMask.find_first_of('#');
		if (iFound == string::npos)
			sOutpuFile += "." + ssFileNum.str();
		else
			sOutpuFile.replace(iFound,1, ssFileNum.str());

		if (bVerbose) cout << "  Generating file '" << sOutpuFile << "' ..." << endl;

		ofstream ofs(sOutpuFile.c_str(), ios_base::out);

		WordList::iterator *itBegin, *itEnd;

		if (iType<2)
			for(WordList::iterator it = *(itBegin = wlst.begin(iFile,true)); it != *(itEnd = wlst.end(iFile,true)); ++it)
				ofs << it->ToString(false, NULL) << "\n";
		else
			for(WordList::iterator it = *(itBegin = wlst.begin(iTakeSplit,iFile==0)); it != *(itEnd = wlst.end(iTakeSplit,iFile==0)); ++it)
				ofs << it->ToString(false, NULL) << "\n";

		ofs.close();
		delete itBegin;	delete itEnd;
	}

	if (bVerbose) {
		time.Print(cout, "  Time needed altogether ");
	}

	return 0;
}

