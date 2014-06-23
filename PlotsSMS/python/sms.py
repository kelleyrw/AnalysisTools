from array import *

class sms():

    def __init__(self, modelname):
        print modelname
        if modelname.find("T1tttt") != -1: self.T1tttt()
        if modelname.find("T1tbbb") != -1: self.T1tbbb()
        if modelname.find("T1ttbb") != -1: self.T1ttbb()
        if modelname.find("T1tttb") != -1: self.T1tttb()
        if modelname.find("T1bbbb") != -1: self.T1bbbb()
        if modelname.find("T2bb"  ) != -1: self.T2bb()
        if modelname.find("T6bbHH") != -1: self.T6bbHH()
        if modelname.find("T2tt"  ) != -1: self.T2tt()
        if modelname.find("T2tb"  ) != -1: self.T2tb()
        if modelname.find("T2bw"  ) != -1: self.T2bw()
        if modelname.find("T2bri" ) != -1: self.T2bri()


    def T1tttt(self):
        # model name
        self.modelname = "T1tttt"
        # decay chain
        self.label= "pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow t#bar{t}#tilde{#chi}^{0}_{1}"
        self.masslabel = ""
        # scan range to plot
        self.Xmin = 400
        self.Xmax = 1500
        self.Ymin = 0
        self.Ymax = 1200
        self.Zmax = 1
        self.Zmin = 5.e-4
        # produce sparticle
        self.sParticle = "m_{#tilde{g}} (GeV)"
        # LSP
        self.LSP = "m_{#tilde{#chi}^{0}} (GeV)"
        # diagonal position: mLSP = mgluino - 2mtop 
        mW = 250
        self.diagX = array('d',[0,20000,self.Xmin])
        self.diagY = array('d',[-mW, 20000-mW,self.Xmax])  
        self.divX = 406
        self.divY = 408
        self.optX = True
        self.optY = True
        
    def T1bbbb(self):
        # model name
        self.modelname = "T1bbbb"
        # decay chain
        self.label= "pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow b#bar{b}#tilde{#chi}^{0}_{1}"
        self.masslabel = ""
        # plot boundary. The top 1/4 of the y axis is taken by the legend
        self.Xmin = 400
        self.Xmax = 1500
        self.Ymin = 0
        self.Ymax = 1200
        self.Zmax = 1
        self.Zmin = 5.e-4
        # produce sparticle
        self.sParticle = "m_{#tilde{g}} (GeV)"
        # LSP
        self.LSP = "m_{#tilde{#chi}^{0}} (GeV)"
        # diagonal position: mLSP = mgluino - 2mtop
        self.diagX = array('d',[0,20000,self.Xmin])
        self.diagY = array('d',[-50, 20000-50,self.Xmax])
        self.divX = 406
        self.divY = 408
        self.optX = True
        self.optY = True

        
    def T2bb(self):
        # model name
        self.modelname = "T2bb"
        # decay chain
        self.label= "pp #rightarrow #tilde{b}#tilde{b}, #tilde{b} #rightarrow b#tilde{#chi}^{0}_{1}"
        self.masslabel = ""
        # plot boundary. The top 1/4 of the y axis is taken by the legend
        self.Xmin = 150
        self.Xmax = 900
        self.Ymin = 0
        self.Ymax = 600
        self.Zmax = 10
        self.Zmin = 1.e-3
        # produce sparticle
        self.sParticle = "m_{#tilde{b}} (GeV)"
        # LSP
        self.LSP = "m_{#tilde{#chi}^{0}} (GeV)"
        # diagonal position: mLSP = mgluino - 2mtop
        self.diagX = array('d',[0,20000,self.Xmin])
        self.diagY = array('d',[-25,20000-25,self.Xmax])
        #self.divX = 407
        self.divX = 409
        self.divY = 408
        self.optX = True
        self.optY = True

        
    def T2tt(self):
        # model name
        self.modelname = "T2tt"
        # decay chain
        self.label= "pp #rightarrow #tilde{t}#tilde{t}, #tilde{t} #rightarrow t#tilde{#chi}^{0}_{1}"
        self.masslabel = ""
        # plot boundary. The top 1/4 of the y axis is taken by the legend
        self.Xmin = 150
        self.Xmax = 900
        self.Ymin = 0
        self.Ymax = 600
        self.Zmax = 10
        self.Zmin = 1.e-3
        # produce sparticle
        self.sParticle = "m_{#tilde{t}} (GeV)"
        # LSP
        self.LSP = "m_{#tilde{#chi}^{0}} (GeV)"
        # diagonal position: mLSP = mgluino - 2mtop
        self.diagX = array('d',[0,20000,self.Xmin])
        self.diagY = array('d',[-100, 20000-100,self.Xmax])
        #self.divX = 407
        self.divX = 409
        self.divY = 408
        self.optX = True
        self.optY = True

        
	# branching ratio independent
    def T2bri(self):
        # model name
        self.modelname = "T2bri"
        # decay chain
        self.label= "pp #rightarrow #tilde{t}#tilde{t}, #tilde{t} #rightarrow t#tilde{#chi}^{0}_{1} / b#tilde{#chi}^{#pm}_{1}"
        self.masslabel = ""
        # plot boundary. The top 1/4 of the y axis is taken by the legend
        self.Xmin = 150
        self.Xmax = 900
        self.Ymin = 0
        self.Ymax = 600
        self.Zmax = 10
        self.Zmin = 1.e-3
        # produce sparticle
        self.sParticle = "m_{#tilde{t}} (GeV)"
        # LSP
        self.LSP = "m_{#tilde{#chi}^{0}} (GeV)"
        # diagonal position: mLSP = mgluino - 2mtop
        self.diagX = array('d',[0,20000,self.Xmin])
        self.diagY = array('d',[-100, 20000-100,self.Xmax])
        #self.divX = 407
        self.divX = 409
        self.divY = 408
        self.optX = True
        self.optY = True

        
    def T2tb(self):
        # model name
        self.modelname = "T2tb"
        # decay chain
        self.label= "pp #rightarrow #tilde{t}#tilde{t}, #tilde{t} #rightarrow t#tilde{#chi}^{0}_{1} / b#tilde{#chi}^{#pm}_{1}"
        self.masslabel = "m_{#tilde{#chi}^{#pm}}-m_{#tilde{#chi}^{0}} = 5 GeV"
        # plot boundary. The top 1/4 of the y axis is taken by the legend
        self.Xmin = 150
        self.Xmax = 900
        self.Ymin = 0
        self.Ymax = 600
        self.Zmax = 10
        self.Zmin = 1.e-3
        # produce sparticle
        self.sParticle = "m_{#tilde{t}} (GeV)"
        # LSP
        self.LSP = "m_{#tilde{#chi}^{0}} (GeV)"
        # diagonal position: mLSP = mgluino - 2mtop
        self.diagX = array('d',[0,20000,self.Xmin])
        self.diagY = array('d',[-100, 20000-100,self.Xmax])
        #self.divX = 407
        self.divX = 409
        self.divY = 408
        self.optX = True
        self.optY = True
        
        
    def T2bw(self):
        # model name
        self.modelname = "T2bw"
        # decay chain
        self.label= "pp #rightarrow #tilde{t}#tilde{t}, #tilde{t} #rightarrow b#tilde{#chi}^{#pm}_{1}"
        self.masslabel = "m_{#tilde{#chi}^{#pm}}-m_{#tilde{#chi}^{0}} = 5 GeV"
        # plot boundary. The top 1/4 of the y axis is taken by the legend
        self.Xmin = 150
        self.Xmax = 900
        self.Ymin = 0
        self.Ymax = 600
        self.Zmax = 10
        self.Zmin = 1.e-3
        # produce sparticle
        self.sParticle = "m_{#tilde{t}} (GeV)"
        # LSP
        self.LSP = "m_{#tilde{#chi}^{0}} (GeV)"
        # diagonal position: mLSP = mgluino - 2mtop
        self.diagX = array('d',[0,20000,self.Xmin])
        self.diagY = array('d',[-100, 20000-100,self.Xmax])
        #self.divX = 407
        self.divX = 409
        self.divY = 408
        self.optX = True
        self.optY = True
        
    def T1tbbb(self):
        # model name
        self.modelname = "T1tbbb"
        # decay chain
        self.label= "pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow tb#tilde{#chi}^{#pm}_{1} / bb#tilde{#chi}^{0}_{1}"
        self.masslabel = "m_{#tilde{#chi}^{#pm}}-m_{#tilde{#chi}^{0}} = 5 GeV"
        # plot boundary. The top 1/4 of the y axis is taken by the legend
        self.Xmin = 400
        self.Xmax = 1500
        self.Ymin = 0
        self.Ymax = 1200
        self.Zmax = 1
        self.Zmin = 5.e-4
        # produce sparticle
        self.sParticle = "m_{#tilde{g}} (GeV)"
        # LSP
        self.LSP = "m_{#tilde{#chi}^{0}} (GeV)"
        # diagonal position: mLSP = mgluino - 2mtop
        self.diagX = array('d',[0,20000,self.Xmin])
        self.diagY = array('d',[-100, 20000-100,self.Xmax])
        self.divX = 406
        self.divY = 408
        self.optX = True
        self.optY = True
        
    def T1ttbb(self):
        # model name
        self.modelname = "T1ttbb"
        # decay chain
        self.label= "pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow tb#tilde{#chi}^{#pm}_{1}"
        self.masslabel = "m_{#tilde{#chi}^{#pm}}-m_{#tilde{#chi}^{0}} = 5 GeV"
        # plot boundary. The top 1/4 of the y axis is taken by the legend
        self.Xmin = 400
        self.Xmax = 1500
        self.Ymin = 0
        self.Ymax = 1200
        self.Zmax = 1
        self.Zmin = 5.e-4
        # produce sparticle
        self.sParticle = "m_{#tilde{g}} (GeV)"
        # LSP
        self.LSP = "m_{#tilde{#chi}^{0}} (GeV)"
        # diagonal position: mLSP = mgluino - 2mtop
        self.diagX = array('d',[0,20000,self.Xmin])
        self.diagY = array('d',[-100, 20000-100,self.Xmax])
        self.divX = 406
        self.divY = 408
        self.optX = True
        self.optY = True
        
        
    def T1tttb(self):
        # model name
        self.modelname = "T1tttb"
        # decay chain
        self.label= "pp #rightarrow #tilde{g}#tilde{g}, #tilde{g} #rightarrow tb#tilde{#chi}^{#pm}_{1} / tt#tilde{#chi}^{0}_{1}"
        self.masslabel = "m_{#tilde{#chi}^{#pm}}-m_{#tilde{#chi}^{0}} = 5 GeV"
        # plot boundary. The top 1/4 of the y axis is taken by the legend
        self.Xmin = 400
        self.Xmax = 1500
        self.Ymin = 0
        self.Ymax = 1200
        self.Zmax = 1
        self.Zmin = 5.e-4
        # produce sparticle
        self.sParticle = "m_{#tilde{g}} (GeV)"
        # LSP
        self.LSP = "m_{#tilde{#chi}^{0}} (GeV)"
        # diagonal position: mLSP = mgluino - 2mtop
        self.diagX = array('d',[0,20000,self.Xmin])
        self.diagY = array('d',[-200, 20000-200,self.Xmax])
        self.divX = 406
        self.divY = 408
        self.optX = True
        self.optY = True

        
    def T6bbHH(self):
        # model name
        self.modelname = "T6bbHH"
        # decay chain
        self.label= "pp #rightarrow #tilde{b}#tilde{b}, #tilde{b}#rightarrow bH#tilde{#chi}^{0}_{1}"
        self.masslabel = "m_{#tilde{#chi}_{2}} = #frac{m_{#tilde{#chi}_{1}}+m_{#tilde{b}}}{2}"
        # plot boundary. The top 1/4 of the y axis is taken by the legend
        self.Xmin = 300
        self.Xmax = 700
        self.Ymin = 0
        self.Ymax = 45 
        # produce sparticle
        self.sParticle = "m_{sbottom} (GeV)"
        # LSP
        self.LSP = "m_{LSP} (GeV)"
        # diagonal position: mLSP = mgluino - 2mhiggs
        self.diagX = array('d',[0,20000, self.Xmin])
        self.diagY = array('d',[-300, 20000-300, self.Xmax])
        self.divX = 404
        self.divY = 409
        self.optX = True
        self.optY = True
