//
// Code to implement some bin boundary calculatiosn starting from
// analyses and bins.
//
#include "Combination/BinBoundaryUtils.h"
#include "Combination/BinUtils.h"
#include "Combination/BinNameUtils.h"
#include "Combination/CommonCommandLineUtils.h"

#include <sstream>
#include <algorithm>
#include <set>

using namespace std;

namespace {
  using namespace BTagCombination;

  typedef map<string, vector<pair<double,double> > > t_bin_list;

  // Error class used to move info between layers when there is a failure.
  class binning_error : public runtime_error {
  public:
    inline binning_error (double lowbinHigh, double highbinLow, const string &message)
      : runtime_error (message.c_str()), _lowbinH (lowbinHigh), _highbinL (highbinLow)
    {}

    double _lowbinH, _highbinL;
  };

  // Go through all the analysis and extract all the bins that are
  // being used.
  //
  // We remove duplicates. We could use a set or similar to do that, but there are so
  // few using the find algoritm is short hand. The duplicates must be removed because when
  // we are looking at a 2D (or n-D) analysis there will always be common values along
  // any particular axis (or there had better be!).
  //
  // On the other hand, identical bins are not allowed!
  t_bin_list extract_bins(const CalibrationAnalysis &ana, bool ignoreExtrapolation)
  {
	  t_bin_list result;
	  set<vector<CalibrationBinBoundary> > binsSeen;

	  for (unsigned int ibin = 0; ibin < ana.bins.size(); ibin++) {
		  const CalibrationBin &bin(ana.bins[ibin]);
		  if (!ignoreExtrapolation || !bin.isExtended) {

			  // Cross check to make sure consistent.
			  if (binsSeen.find(bin.binSpec) != binsSeen.end()) {
				  ostringstream errtxt;
				  errtxt << "The bin " << OPBinName(bin) << "has been seen twice in the analysis " << OPFullName(ana);
				  throw runtime_error(errtxt.str());
			  }
			  binsSeen.insert(bin.binSpec);

			  for (unsigned int iboundary = 0; iboundary < bin.binSpec.size(); iboundary++) {
				  const CalibrationBinBoundary &bound(bin.binSpec[iboundary]);

				  pair<double, double> bp = make_pair(bound.lowvalue, bound.highvalue);
				  if (find(result[bound.variable].begin(), result[bound.variable].end(), bp)
					  == result[bound.variable].end())
					  result[bound.variable].push_back(bp);
			  }
		  }
	  }

	  return result;
  }

  // Helper function to order bins by their lower boundary in the "sort" algorithm.
  bool compare_first_of_pair (const pair<double, double> &i1, const pair<double, double> &i2)
  {
    return i1.first < i2.first;
  }

  // Given all bins make sure the are adjacent and create just a list of bin boundaries much
  // like what you would put in a histogram.
  vector<double> get_bin_boundaries_hist(const vector<pair<double, double> > &allbins)
  {
	  // Put them in order
	  vector<pair<double, double> > acopy(allbins);
	  sort(acopy.begin(), acopy.end(), compare_first_of_pair);

	  // Now, extract the boundaries
	  vector<double> result;
	  pair<double, double> last;
	  for (unsigned int i = 0; i < acopy.size(); i++) {
		  if (acopy[i].first == acopy[i].second) {
			  ostringstream errtext;
			  errtext << "Bins can't be infinitely thing - lower and upper have the same boundary: " << acopy[i].first;
			  throw binning_error(acopy[i].first, acopy[i].second, errtext.str());
		  }
		  if (result.size() == 0) {
			  result.push_back(acopy[i].first);
		  }
		  else {
			  if (acopy[i] == last) {
				  ostringstream errtxt;
				  throw binning_error(last.first, last.second, "Duplicate bin boundaries!");
			  }
			  if (last.second != acopy[i].first) {
				  ostringstream errtxt;
				  errtxt << "Bins must be adjacent and exclusive - "
					  << " lower bin's upper boundary (" << last.second << ")"
					  << " and upper bins' lower boundary (" << acopy[i].first << ") need to be the same";
				  throw binning_error(last.second, acopy[i].first, errtxt.str());
			  }
			  result.push_back(acopy[i].first);
		  }

		  last = acopy[i];
	  }
	  result.push_back(last.second);
	  return result;
  }

  CalibrationBinBoundary get_bin_boundary_for (const CalibrationBin &bin, const string &boundaryname)
  {
    for (unsigned int i = 0; i < bin.binSpec.size(); i++) {
      if (bin.binSpec[i].variable == boundaryname)
	return bin.binSpec[i];
    }
    return CalibrationBinBoundary();
  }

  // Get a list of all bins that match a criteria
  vector<CalibrationBin> get_all_bins_matching (const CalibrationAnalysis &ana, const string &binname,
						double lowbinH, double highbinL)
  {
    vector<CalibrationBin> result;

    for (unsigned int i = 0; i < ana.bins.size(); i++) {
      CalibrationBinBoundary b (get_bin_boundary_for(ana.bins[i], binname));
      if (b.lowvalue == highbinL || b.highvalue == lowbinH)
	result.push_back(ana.bins[i]);
    }

    return result;
  }

  // Return the bin pairs
  vector<pair<double, double> > make_bin_boundaries(const vector<double> &blist)
  {
    vector<pair<double, double> > result;;
    if (blist.size() < 2)
      return result;

    double low = blist[0];
    for (unsigned int i = 1; i < blist.size(); i++){
      result.push_back(make_pair(low, blist[i]));
      low = blist[i];
    }
    
    return result;
  }

  // Can we find this exact bin in here?
  bool find_exact_match(const vector<pair<double, double> > &blist, const pair<double, double> &bin) {
    for (unsigned int i = 0; i < blist.size(); i++) {
      if (bin == blist[i])
	return true;
    }
    return false;
  }

  // True if a bin point is in one of these bins.
  bool is_in_bin (const pair<double, double> &bin, double point)
  {
    return bin.first < point && bin.second > point;
  }

  // Make sure there is no overlap... Asumme that this is not
  // the identical bin.
  bool spans_bins (const vector<pair<double, double> > &blist, const pair<double, double> &bin)
  {
    for (unsigned int i = 0; i < blist.size(); i++) {
      bool inf = is_in_bin(blist[i], bin.first);
      bool ins = is_in_bin(blist[i], bin.second);

      // If either one has a foot inside, or both feet inside, then
      // this guy is spannign this bin.
      if ((inf && !ins)
	  || (!inf && ins)
	  || (inf && ins))
	return true;

      // Check to see if the bin is outside...
      if (blist[i].first >= bin.first
	  && blist[i].second <= bin.second)
	return true; // Bigger than the whole bin!!
    }
    return false;
  }
  
}

namespace BTagCombination {

  ///
  /// The bin_boudnaries object
  ///

  int bin_boundaries::get_xaxis_bin (const vector<CalibrationBinBoundary> &bin_spec) const
  {
    return find_bin (get_xaxis(), bin_spec);
  }

  int bin_boundaries::get_yaxis_bin (const vector<CalibrationBinBoundary> &bin_spec) const
  {
    return find_bin (get_yaxis(), bin_spec);
  }

  // return a list of the axes we know about.
  vector<string> bin_boundaries::axis_names() const
  {
    vector<string> result;
    for(map<string,vector<double> >::const_iterator itr = _axes.begin(); itr != _axes.end(); itr++) {
      result.push_back(itr->first);
    }
    return result;
  }

  // return bin boundaries for aparticular bin. Bomb if we can't find them.
  vector<double> bin_boundaries::get_axis_bins(const string &axis_name) const
  {
    vector<double> result;
    map<string,vector<double> >::const_iterator axis = _axes.find(axis_name);
    if (axis == _axes.end())
      throw runtime_error ((string("This analysis has no axis called '") + axis_name + "'").c_str());
    return axis->second;
  }

  int bin_boundaries::find_bin (const pair<string, vector<double> > &axis_info,
			 const vector<CalibrationBinBoundary> &bin_spec) const
  {
    CalibrationBinBoundary spec = BinBoundaryUtils::find_spec(bin_spec, axis_info.first);
    for (unsigned int ibin = 0; ibin < axis_info.second.size(); ibin++) {
      if (spec.lowvalue == axis_info.second[ibin]) {
	return ibin + 1; // Remamer, root is offset by 1 its bin numbers!!
      }
    }
    ostringstream error;
    error << "Unable to find bin wiht lower boundary '" << spec.lowvalue << "' in axis '" << axis_info.first << "'.";
    throw runtime_error (error.str().c_str());
  }

  // Grab the boundaries from the analysis, and put them into
  // an object that knows how to create historams, etc., for teh CDI.
  // Fail fairly resolutely if we find problems (overlaps, thin bins, etc.).
  bin_boundaries calcBoundaries (const CalibrationAnalysis &ana, bool ignoreExtrap)
  {
    // Find all the bins that are in the analysis
    t_bin_list raw_bins = extract_bins(ana, ignoreExtrap);
    if (raw_bins.size() > 2) {
      throw runtime_error(("Analysis '" + ana.name + "' has more than 2 bins!").c_str());
    }
    if (raw_bins.size() == 0) {
      throw runtime_error(("Analysis '" + ana.name + "' has no bins!").c_str());
    }

    // For each variable, get a set of bin boundaries

    bin_boundaries result;
    bool errorSeen = false;
    ostringstream errmsg;
    errmsg << "Found problems with binning: ";
    for (t_bin_list::const_iterator ibin = raw_bins.begin(); ibin != raw_bins.end(); ibin++) {
      try {
	result.add_axis(ibin->first, get_bin_boundaries_hist(ibin->second));
      } catch (binning_error &e){
	errmsg << endl;
	errmsg << "    " << e.what() << endl;

	// Now we have to find all the possibly offending bins, and then
	// print them out in the standard printing format that can be used as part
	// of our --ignore command line.

	errmsg << "      One of the following analysis/bins is in error - please use --ignore" << endl;

	vector<CalibrationBin> badbins(get_all_bins_matching(ana, ibin->first, e._lowbinH, e._highbinL));
	for (unsigned int i = 0; i < badbins.size(); i++) {
	  errmsg << "      -> " << OPIgnoreFormat(ana, badbins[i]) << endl;
	}

	errorSeen = true;
      }
    }

    if (errorSeen) {
      throw bin_boundary_error (errmsg.str().c_str());
    }

    return result;
  }

  //
  // Check to see if bins from multiple analyses are consistent. If not,
  // then total failure and throw!
  //
  // We know a few things before we show up here.
  //  1) Each analysis has no gaps in the bin boundaries
  //
  // It is ok if the binning dosen't match in the following cases:
  //  1) an analysis has a bin the other is missing
  //
  void checkForConsitentBoundaries (const std::vector<bin_boundaries> &boundaries)
  {
    // Setup
    if (boundaries.size() <= 1)
      return;

    const bin_boundaries &proto(boundaries[0]);
    set<string> panames;
    {
      vector<string> allnames(proto.axis_names());
      panames.insert(allnames.begin(), allnames.end());
    }

    // First the easy stuff - everyone must have the same # of axes and
    // same axes - the binning must be identical.

    for (unsigned int i = 1; i < boundaries.size(); i++) {
      if (boundaries[0].size() != boundaries[i].size()) {
	throw runtime_error ("Analyses to combine don't have identical binning: number of binning axes differs");
      }
      vector<string> anames (boundaries[i].axis_names());
      for (unsigned int i_n = 0; i_n < anames.size(); i_n++) {
	if (panames.find(anames[i_n]) == panames.end()) {
	  ostringstream err;
	  err << "Not all analyses have a bin axis '" << anames[i_n] << "'.";
	  throw runtime_error (err.str().c_str());
	}

	vector<pair<double, double> > pblist (make_bin_boundaries(proto.get_axis_bins(anames[i_n])));
	vector<pair<double, double> > tblist (make_bin_boundaries(boundaries[i].get_axis_bins(anames[i_n])));

	for (unsigned int i_b = 0; i_b < tblist.size(); i_b++) {
	  pair<double, double> tbin (tblist[i_b]);
	  if (!find_exact_match(pblist, tbin)
	      && spans_bins(pblist, tbin)) {
	    ostringstream err;
	    err << "Bins in '" << anames[i_n] << "' have inconsistent boundaries ("
		<< tbin.first << "-" << anames[i_n] << "-" << tbin.second << ")";
	    throw runtime_error (err.str().c_str());
	  }
	}
      }
    }
  }

  //
  // Look at the analyses and make sure they are "consistent" accross
  // the various analyses and ok to combine.
  //
  // THings we look for:
  //  - If a systematic is uncorrelated in one analysis it must be uncorrelated in
  //    all analyses.
  //
  void checkForConsistentAnalyses (const std::vector<CalibrationAnalysis> &anas)
  {
    // First job is to sort things by "combination" group.
    typedef map<string, vector<CalibrationAnalysis> > t_cmap;
    t_cmap binned (BinAnalysesByJetTagFlavOp(anas));

    map<string, bool> sysCorrelatedTracker; // Are they consistent?
    for (t_cmap::const_iterator itr = binned.begin(); itr != binned.end(); itr++) {
      for (unsigned int i_ana = 0; i_ana < itr->second.size(); i_ana++) {
	for (unsigned int i_b = 0; i_b < itr->second[i_ana].bins.size(); i_b++) {
	  const CalibrationBin &b(itr->second[i_ana].bins[i_b]);
	  for (unsigned int i_s = 0; i_s < b.systematicErrors.size(); i_s++) {
	    const SystematicError &e (b.systematicErrors[i_s]);
	    map<string,bool>::const_iterator i_sys = sysCorrelatedTracker.find(e.name);
	    if (i_sys == sysCorrelatedTracker.end()) {
	      sysCorrelatedTracker[e.name] = e.uncorrelated;
	    } else if (e.uncorrelated != i_sys->second) {
	      ostringstream msg;
	      msg << "Systematic error '" << i_sys->first << "' marked as correlated in some analyses and uncorrelated in others! It must be consistent.";
	      throw runtime_error (msg.str().c_str());
	    }
	  }
	}
      }
    }
  }

  // Find a bin boundary with the same name.
  set<CalibrationBinBoundary>::const_iterator findBinBoundary(const string &name, const set<CalibrationBinBoundary> &b2) {
    for (set<CalibrationBinBoundary>::const_iterator i = b2.begin(); i != b2.end(); i++) {
      if (i->variable == name)
	return i;
    }
    return b2.end();
  }

  // Make sure two bins don't overlap at all!
  bool noOverlap (const CalibrationBinBoundary &b1, const CalibrationBinBoundary &b2)
  {
    if (b1.variable != b2.variable)
      return true;
    if (b1.lowvalue >= b2.highvalue)
      return true;
    if (b2.lowvalue >= b1.highvalue)
      return true;
    return false;
  }

  // Make sure the bins are somehow compatible (i.e. they have orthoginal bins in at least one case).
  // Throw a nice detailed error when this happens.
  void checkForOrthogonal (const set<CalibrationBinBoundary> &b1, const set<CalibrationBinBoundary> &b2)
  {
    for (set<CalibrationBinBoundary>::const_iterator i_orig = b1.begin(); i_orig != b1.end(); i_orig++) {
      set<CalibrationBinBoundary>::const_iterator i_final = findBinBoundary (i_orig->variable, b2);
      if (i_final != b2.end()) {
	if (noOverlap(*i_orig, *i_final))
	  return; // At least one was orthogonal, which is what we need.
      }
    }

    //
    // If we got here, then we have a problem! We need to make a nice error message.
    //

    ostringstream err;
    err << "The following binning boundaries are not compatible in a bin-by-bin fit:" << endl;
    err << "  - " << OPBinName(b1) << endl;
    err << "  - " << OPBinName(b2) << endl;

    throw runtime_error(err.str());
  }

  //
  // Search to make sure that all bins are compatible when we are doing a bin-by-bin combination.
  // In this case, we are just doing isolated bins, so nothign can overlap.
  //
  void checkForConsistenBoundariesBinByBin (const std::vector<CalibrationAnalysis> &anas)
  {
    // Get a list of all the bins in the analyses.
    typedef set<set<CalibrationBinBoundary> > t_setOfBoundaries;
    t_setOfBoundaries allBins (listAllBins(anas));

    // Loop through them all, and see if there is any cross-matching.
    for (t_setOfBoundaries::const_iterator i_test = allBins.begin(); i_test != allBins.end(); i_test++) {
      // Match to each one - this is an n^2 operation. :(
      for (t_setOfBoundaries::const_iterator i_against = allBins.begin(); i_against != allBins.end(); i_against++) {
	if (*i_against != *i_test)
	  checkForOrthogonal (*i_test, *i_against);
      }
    }
  }

  //
  // Look for valid correlations - bomb if there is an invalid one.
  //
  void checkForValidCorrelations (const CalibrationInfo &info)
  {
    //
    // Get the analysis names for all analyses...
    //
    set<string> goodBins;
    for (unsigned int i = 0; i < info.Analyses.size(); i++) {
      for (unsigned int b = 0; b < info.Analyses[i].bins.size(); b++) {
	goodBins.insert(OPIgnoreFormat(info.Analyses[i], info.Analyses[i].bins[b]));
      }
    }

    //
    // Now, loop through the complete set of correlations and see if we can find
    // what we need. Messy, but a good way to maintian th eformatting of these strings
    // in only one place.
    //

    for (size_t i = 0; i < info.Correlations.size(); i++) {
      const AnalysisCorrelation &ac(info.Correlations[i]);
      for (size_t b = 0; b < ac.bins.size(); b++) {
	const BinCorrelation &c(ac.bins[b]);
	CalibrationAnalysis t_ana;
	t_ana.name = ac.analysis1Name;
	t_ana.flavor = ac.flavor;
	t_ana.tagger = ac.tagger;
	t_ana.operatingPoint = ac.operatingPoint;
	t_ana.jetAlgorithm = ac.jetAlgorithm;

	CalibrationBin t_bin;
	t_bin.binSpec = c.binSpec;

	string cname (OPIgnoreFormat(ac, c));
	string aname (OPIgnoreFormat(t_ana, t_bin));
	if (goodBins.find(aname) == goodBins.end()) {
	  ostringstream out;
	  out << "The '" << t_ana.name << "' analysis for the correlation "
	      << cname << " is not known.";
	  throw runtime_error (out.str().c_str());
	}

	t_ana.name = ac.analysis2Name;
	aname = OPIgnoreFormat(t_ana, t_bin);
	if (goodBins.find(aname) == goodBins.end()) {
	  ostringstream out;
	  out << "The '" << t_ana.name << "' analysis for the correlation "
	      << cname << " is not known.";
	  throw runtime_error (out.str().c_str());
	}

	if (ac.analysis1Name == ac.analysis2Name) {
	  ostringstream out;
	  out << "Can't have a correlations between the same analyses: " << cname;
	  throw runtime_error (out.str().c_str());
	}
      }
    }
  }

}
