#include "writer/Wstats.h"

writer::Wstats::Wstats(std::string name_file): write_parametres(5) {
	ofstat.open(name_file); 
} 

void writer::Wstats::print_all_statistics(int stage, const Statistics& info, const ProblemInstance& cfg, const MBGraph& graph) { 
	if (stage == 0) { 
		println("Initial graph:");
	}  else { 
		println("After Stage " + toString(stage) + " graph:");
	} 

	print_complete_edges(graph);
	print_connected_components(graph);
	print_rear_characters(info.get_compl_stat(graph)); 
#ifndef VERSION2
	print_estimated_dist(stage, cfg, graph);
#endif
	print_fair_edges(graph);
	print_not_compl_characters(info.get_no_compl_stat(graph)); 
} 


////////////////////////////////////////////////////////
void writer::Wstats::print_connected_components(const MBGraph& MBG) {
	// count connected components in the graph
	equivalence<vertex_t> C;
	
	for(auto is = MBG.begin_vertices(); is != MBG.end_vertices(); ++is) {
		C.addrel(*is, *is);
	} 

	for(size_t i = 0; i < MBG.size_graph(); ++i) {
		for(auto il = MBG.LG[i].cbegin(); il != MBG.LG[i].cend(); ++il) {
			C.addrel(il->first, il->second);
		}
	}

	C.update();

	std::map<vertex_t, std::set<vertex_t> > cls;
	C.get_eclasses(cls);

	std::map<size_t, size_t> stx;
	for(auto ic = cls.begin(); ic != cls.end(); ++ic) {
		++stx[ic->second.size()];
	}

	ofstat << "... connected components:";
	for(auto is = stx.begin(); is != stx.end(); ++is) {
		ofstat << " " << is->first << "^" << is->second;
	}
	ofstat << std::endl;
}

void writer::Wstats::histStat() { //FIXME
	ofstat << std::endl << "Total number of 2-breaks: " << TwoBreak::History.size() << std::endl;

 	std::map<Mcolor, size_t> n2br;

	for(auto il = TwoBreak::History.begin(); il != TwoBreak::History.end(); ++il) {
		++n2br[il->MultiColor];
	}

	for(auto im = n2br.begin(); im != n2br.end(); ++im) {
		ofstat << genome_match::mcolor_to_name(im->first) << "\t" << im->second << std::endl;
	}
	
	ofstat << std::endl;
}
////////////////////////////////////////////////////////
void writer::Wstats::print_fair_edges(const MBGraph& MBG) {
	// output H-subgraphs count
	ofstat << std::endl << "% Fair multi-edges count: " << std::endl << std::endl;

	std::map<std::pair<Mcolor, Mcolor>, size_t> Hcount = MBG.get_count_Hsubgraph();
	std::list<Mcolor> HCrow; 	//the set of multicolors that appear in H-subraphs

#ifndef HG_TONLY // adding other colors
	std::set<Mcolor> proc;
	for(auto ih = Hcount.cbegin(); ih != Hcount.cend(); ++ih) {
		if (!member(proc, ih->first.first)) {
			HCrow.push_back(ih->first.first);
			proc.insert(ih->first.first);
		}

		if (!member(proc, ih->first.second)) {
			HCrow.push_back(ih->first.second);
			proc.insert(ih->first.second);
		}
	}
#endif
	ofstat << "\\begin{table}[h]" << std::endl;
	ofstat << "\\centering \\begin{tabular}{|c||"; //FIXME::why twice
	for(int i = 0; i < HCrow.size(); ++i) { 
		ofstat << "c|";
	} 
	ofstat << "}" << std::endl;
	ofstat << "\\hline" << std::endl;

	for(auto ic = HCrow.cbegin(); ic != HCrow.cend(); ++ic) {
		ofstat << " & ${";
		if (MBG.is_T_consistent_color(*ic)) {
			ofstat << "\\bf ";
		} 
		ofstat <<  genome_match::mcolor_to_name(*ic) << "+}$";
	}

	println("\\\\");
	println("\\hline \\hline");

	for(auto Q1 = HCrow.cbegin(); Q1 != HCrow.cend(); ++Q1) {
		ofstat << "${";
		if (MBG.is_T_consistent_color(*Q1)) {
			ofstat << "\\bf ";
		} 
		ofstat <<   genome_match::mcolor_to_name(*Q1) << "+}$"; 

		for(auto Q2 = HCrow.cbegin(); Q2 != HCrow.cend(); ++Q2) {
			ofstat << " & ";

			if (Hcount.find(std::make_pair(*Q1, *Q2)) != Hcount.end()) {
				ofstat << " "; //"${";

				if (MBG.AreAdjacentBranches(*Q1, *Q2)) { 
					ofstat << "{\\cellcolor[gray]{.9}}";
				} 

#ifdef HG_TONLY
				if (member(BranchLen, genome_match::mcolor_to_name(*Q1)) && member(BranchLen,  genome_match::mcolor_to_name(*Q2))) {
					long t = (long) (1e5 * Hcount[std::make_pair(*Q1, *Q2)] / (double) BranchLen[genome_match::mcolor_to_name(*Q1)] / (double) BranchLen[genome_match::mcolor_to_name(*Q2)]); 
					ofstat << "$" << t << "\\cdot 10^{-5}$ ";
				}
#else
				ofstat << Hcount[std::make_pair(*Q1, *Q2)] << " "; 
#endif
			}

			if (*Q1 == *Q2) {
				ofstat << "$\\star$";
			}
		}
		ofstat << " \\\\" << std::endl;
		ofstat << "\\hline" << std::endl;
	}
	print_close_table(false);
}

void writer::Wstats::print_complete_edges(const MBGraph& graph) { 
	size_t nc = 0;
	ofstat << "... complete multiedges:";
	for(auto it = graph.begin_vertices(); it != graph.end_vertices(); ++it) {
		//multimularcs_t M = graph.get_adjacent_multiedges_v2(*it);
		mularcs_t M = graph.get_adjacent_multiedges(*it);
		if (M.size() == 1 && M.begin()->second.size() == graph.size_graph() && (*it < M.begin()->first || M.begin()->first == Infty)) {
			ofstat << " " << *it << "~" << M.begin()->first;
			++nc;
		}
    	}
	ofstat << "\t(total: " << nc << ")" << std::endl;
} 

void writer::Wstats::print_rear_characters(const std::vector<std::string>& info) { 
	ofstat << std::endl;
	ofstat << "% Rearrangement characters:" << std::endl << std::endl;
	print_start_table(5); 
	ofstat << "Multicolors & multiedges & simple vertices & simple multiedges & simple paths+cycles & irreg. multiedges\\\\" << std::endl;
	ofstat << "\\hline" << std::endl;

	for(auto im = info.cbegin(); im != info.cend(); ++im) {
		ofstat << *im << "\\\\" << std::endl;
	}
	print_close_table();	
} 

void writer::Wstats::print_not_compl_characters(const std::vector<std::string>& info) { 
	if (!info.empty()) { 
		ofstat << std::endl;
		ofstat << "% Edges for duplication block:" << std::endl << std::endl;
		print_start_table(1);
		ofstat << "Multicolors & multiedges\\\\" << std::endl;
		ofstat << "\\hline" << std::endl;
		for(auto im = info.cbegin(); im != info.cend(); ++im) {
			ofstat << *im << "\\\\" << std::endl;
		}
		print_close_table();	
	} 
} 

void writer::Wstats::print_estimated_dist(size_t stage, const ProblemInstance& cfg, const MBGraph& graph) { 
	ofstat << "% Estimated distances:" << std::endl << std::endl;
	print_start_table(graph.size_graph());
	ofstat << "Stage " << stage;
	for(auto it = cfg.cbegin_name_genome(); it != cfg.cend_name_genome(); ++it) { 
		ofstat << " & " << (*it); 
	} 
	ofstat << " \\\\" << std::endl << "\\hline" << std::endl;

	size_t i = 0; 
	for(auto it = cfg.cbegin_name_genome(); it != cfg.cend_name_genome(); ++it, ++i) {
		ofstat << (*it) << " & "; 
		
		for(size_t j = 0; j < graph.size_graph(); ++j) {
			if (j > i) {
				ofstat << genome_dist(MBG.LG[i], MBG.LG[j], MBG.get_obverce_graph())[2]; //FIXME
			}

			if (j == graph.size_graph() - 1) { 
				ofstat << "\\\\";
			} else { 
				ofstat << " & ";			
			} 
		}
		ofstat << std::endl;
	}
	print_close_table();
} 

void writer::Wstats::print_start_table(size_t count_column) { 
	ofstat << "\\begin{table}[h]" << std::endl;
	ofstat << "\\centering \\begin{tabular}{|c|";
	for(size_t i = 0; i < count_column; ++i) { 
		ofstat << "c|";
	} 
	ofstat << "}" << std::endl;
	ofstat << "\\hline" << std::endl;	
} 

void writer::Wstats::print_close_table(bool flag) { 
	if (flag) { 
		ofstat << "\\hline" << std::endl;
	} 
	ofstat << "\\end{tabular}" << std::endl;
	ofstat << "\\end{table}" << std::endl;
	ofstat << std::endl;
} 


