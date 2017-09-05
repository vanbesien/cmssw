// -*- C++ -*-
//
// Package:    ProdTutorial/TcdsRawToDigi
// Class:      TcdsRawToDigi
// 
/**\class TcdsRawToDigi TcdsRawToDigi.cc ProdTutorial/TcdsRawToDigi/plugins/TcdsRawToDigi.cc

 Description:  Producer to unpack lumi nibble from TCDS 

*/
//
// Original Author:  Chris Palmer
// Improved by    :  Salvatore Di Guida
//         Created:  Thu, 28 May 2015 19:54:56 GMT
//
//


// system include files
#include <memory>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDHeader.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"

#include "DataFormats/Scalers/interface/BSTRecord.h"

#include "EventFilter/FEDInterface/interface/GlobalEventNumber.h"
#include "EventFilter/FEDInterface/interface/FED1024.h"

using namespace std;

//
// class declaration
//


class TcdsRawToDigi : public edm::stream::EDProducer<> {
   public:
      explicit TcdsRawToDigi(const edm::ParameterSet&);
      ~TcdsRawToDigi();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      
      edm::EDGetTokenT<FEDRawDataCollection> dataToken_;
      
};

TcdsRawToDigi::TcdsRawToDigi(const edm::ParameterSet& iConfig)
{
    edm::InputTag dataLabel = iConfig.getParameter<edm::InputTag>("InputLabel");
    dataToken_=consumes<FEDRawDataCollection>(dataLabel);
    produces<int>( "nibble" ).setBranchAlias( "nibble");
    produces<int64_t>( "triggerCount" ).setBranchAlias( "triggerCount");
    produces<BSTRecord>( "bstRecord" ).setBranchAlias( "bstRecord" );
}


TcdsRawToDigi::~TcdsRawToDigi()
{
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void TcdsRawToDigi::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;

    edm::Handle<FEDRawDataCollection> rawdata;  
    iEvent.getByToken(dataToken_,rawdata);

    int nibble=-99;
    int triggerCount = -99;
    BSTRecord bstRecord;
    if( rawdata.isValid() ) {
        const FEDRawData& tcdsData = rawdata->FEDData(FEDNumbering::MINTCDSuTCAFEDID);
        if(tcdsData.size()>0){
            evf::evtn::TCDSRecord tcdsRecord(tcdsData.data());
            nibble = (int)tcdsRecord.getHeader().getNibble();
            triggerCount = (int64_t)tcdsRecord.getHeader().getTriggerCount();
            uint64_t gpsh = tcdsRecord.getBST().getGpsTimeHigh();
            uint32_t gpsl = tcdsRecord.getBST().getGpsTimeLow();
            unsigned long long gpstime = static_cast<unsigned long long>((gpsh << 32) + gpsl);
            unsigned char bstMaster = static_cast<unsigned char>(tcdsRecord.getBST().getBstMasterBireserved16() >> 8);
            uint32_t turnCountHigh = tcdsRecord.getBST().getTurnCountHigh();
            uint16_t turnCountLow = tcdsRecord.getBST().getTurnCountLow();
            unsigned int turnCount = static_cast<unsigned int>((turnCountHigh << 16) + turnCountLow);
            uint32_t lhcFillHigh = tcdsRecord.getBST().getLhcFillHigh();
            uint16_t lhcFillLow = tcdsRecord.getBST().getLhcFillLow();
            unsigned int lhcFill = static_cast<unsigned int>((lhcFillHigh << 16) + lhcFillLow);
            unsigned short beamMode = static_cast<unsigned short>(tcdsRecord.getBST().getBeamMode());
            unsigned char particleType1 = static_cast<unsigned char>(tcdsRecord.getBST().getParticleTypes() & 0xFF);
            unsigned char particleType2 = static_cast<unsigned char>(tcdsRecord.getBST().getParticleTypes() >> 8);
            unsigned short beamMomentum = static_cast<unsigned short>(tcdsRecord.getBST().getBeamMomentum());
            // Sometimes it comes in with all bits 1: (2**16 - 1 = 65535)
            if (beamMomentum == 65535) {
              beamMomentum = 0;
            // Momentum used to come in just correctly, but since 2017-03-29
            // the machine is sending those bits with a correction factor of
            // 1GeV/120MeV = 8.33333
            } else if (iEvent.run() >= 290413) {
              beamMomentum = beamMomentum * 120 / 1000;
            }
            unsigned int intensityBeam1 = static_cast<unsigned int>(tcdsRecord.getBST().getIntensityBeam1());
            unsigned int intensityBeam2 = static_cast<unsigned int>(tcdsRecord.getBST().getIntensityBeam2());
            bstRecord.set(gpstime, bstMaster, turnCount, lhcFill, beamMode, particleType1, particleType2, beamMomentum, intensityBeam1, intensityBeam2);
        } else {
            nibble=-2;
            triggerCount=-2;
        }
    } else {
        nibble=-1;
        triggerCount=-1;
    }
    //std::cout<<"nibble is "<<nibble<<std::endl;

    iEvent.put(std::make_unique<int>(nibble), "nibble");
    iEvent.put(std::make_unique<int64_t>(triggerCount), "triggerCount");
    iEvent.put(std::make_unique<BSTRecord>(bstRecord), "bstRecord");
}

 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TcdsRawToDigi::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("InputLabel",edm::InputTag("rawDataCollector"));
    descriptions.add("tcdsRawToDigi", desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TcdsRawToDigi);
