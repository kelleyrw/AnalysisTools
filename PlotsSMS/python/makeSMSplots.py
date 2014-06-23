import sys
from inputFile import *
from smsPlotXSEC import *
from smsPlotCONT import *
from smsPlotBrazil import *
import ROOT as rt

if __name__ == '__main__':
    # read input arguments
    filename = sys.argv[1]
    modelname = sys.argv[1].split("/")[-1].split("_")[0]
    analysisLabel = sys.argv[1].split("/")[-1].split("_")[1]
    outputname = sys.argv[2]

    # read the config file
    fileIN = inputFile(filename)
    
	# no graphics
    rt.gROOT.SetBatch(True)

    # classic temperature histogram
    xsecPlot = smsPlotXSEC(modelname, fileIN.HISTOGRAM, fileIN.OBSERVED, fileIN.EXPECTED, fileIN.EXPECTED2, fileIN.ENERGY, fileIN.LUMI, fileIN.PRELIMINARY, fileIN.BOXES, "")
    xsecPlot.Draw()
    xsecPlot.Save("%sXSEC" %outputname)

    # only lines
    contPlot = smsPlotCONT(modelname, fileIN.HISTOGRAM, fileIN.OBSERVED, fileIN.EXPECTED, fileIN.EXPECTED2, fileIN.ENERGY, fileIN.LUMI, fileIN.PRELIMINARY, fileIN.BOXES, "")
    contPlot.Draw()
    contPlot.Save("%sCONT" %outputname)

    # brazilian flag (show only 1 sigma)
    brazilPlot = smsPlotBrazil(modelname, fileIN.HISTOGRAM, fileIN.OBSERVED, fileIN.EXPECTED, fileIN.EXPECTED2, fileIN.ENERGY, fileIN.LUMI, fileIN.PRELIMINARY, fileIN.BOXES, "")
    brazilPlot.Draw()
    brazilPlot.Save("%sBAND" %outputname)
