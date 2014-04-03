import FWCore.ParameterSet.Config as cms

## process to parse (THIS SHOULD NOT CHANGE)
process = cms.PSet()

## ------------------------------------------------------------- #
## Parameters for the cms2tools_keep_branches utility 
## ------------------------------------------------------------- #

process.cms2tools_drop_branches = cms.PSet(

	## max number of events to run on (-1 means all)
	max_events = cms.int64(-1),

	## tree name
	tree_name = cms.string("Events"),
	
	## path to the analysis
	input_files = cms.vstring(
		"/nfs-7/userdata/rwkelley/cms2/DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball_Summer12_DR53X-PU_S10_START53_V7A-v1.root",
	),

	## output file name 
	output_file = cms.string("test.root"),

	## selection (same as TTree::Draw)
# 	selection = cms.string(""),
	selection = cms.string(
		"(Sum$(genps_status==3 && genps_id==11)>=1 && Sum$(genps_status==3 && genps_id==-11)>=1) || "
		"(Sum$(genps_status==3 && genps_id==13)>=1 && Sum$(genps_status==3 && genps_id==-13)>=1) || "
		"(Sum$(genps_status==3 && genps_id==15)>=1 && Sum$(genps_status==3 && genps_id==-15)>=1)"
	),

	## aliases to branches to keep (overides anything in drop_alias_names)
	## have to use regegular expression
	## see: http://www.cplusplus.com/reference/regex/ECMAScript/
	keep_alias_names = cms.vstring(
		"genps(.*)",
# 		"evt_(.*)",
	),
)
