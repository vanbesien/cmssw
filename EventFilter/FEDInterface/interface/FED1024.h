#ifndef EVF_FEDINTERFACE_FED1024_H
#define EVF_FEDINTERFACE_FED1024_H

// Needed for sizeof(fedh_t):
#include "EventFilter/FEDInterface/interface/fed_header.h"

/*
 * More documentation:
 *   https://twiki.cern.ch/twiki/bin/view/CMS/TcdsEventRecord
 *
 * This class defines the TCDS Record,
 *   which is a container, composed of the
 *   - TCDS FED Header
 *   - TCDS Header
 *   - TCDS L1A History
 *   - TCDS BST
 */

namespace evf{
  namespace evtn{

    class TCDSFEDHeader {
     public:
      // Source data structure:
      union tcdsfedheader {
        uint64_t const word;
        struct {
          uint32_t const sourceid;
          uint32_t const eventid;
        } const header;
      };
      // Constructor:
      TCDSFEDHeader(const unsigned char* pbytes) :
        fedheader_((tcdsfedheader*)(pbytes)) {}
      // Accessor:
      tcdsfedheader const& getFEDHeader() const { return *fedheader_; }
      // Direct accessors:
      uint32_t const& getSourceId() const { return fedheader_->header.sourceid; }
      uint32_t const& getEventId() const { return fedheader_->header.eventid; }

     private:
      tcdsfedheader* fedheader_;
    };


    class TCDSHeader {
     public:
      // Source data structures:
      union tcdssizes {
        uint64_t const sizeword;
        struct {
          unsigned char const headerSize;
          unsigned char const summarySize;
          unsigned char const L1AhistSize;
          unsigned char const BSTSize;
          unsigned char const reserved0;
          unsigned char const reserved1;
          unsigned char const reserved2;
          unsigned char const BGOSize;
        } const size;
      };
      union tcdsheader {
        uint64_t const words[11];
        struct {
          uint64_t const macAddress;
          uint32_t const sw;
          uint32_t const fw;
          uint32_t const reserved0;
          uint32_t const format;
          uint32_t const runNumber;
          uint32_t const reserved1;
          uint32_t const activePartitions2;
          uint32_t const reserved2;
          uint32_t const activePartitions0;
          uint32_t const activePartitions1;
          uint32_t const nibble;
          uint32_t const lumiSection;
          uint16_t const nibblesPerLumiSection;
          uint16_t const triggerTypeFlags;
          uint16_t const reserved5;
          uint16_t const inputs;
          uint16_t const bcId;
          uint16_t const orbitLow;
          uint32_t const orbitHigh;
          uint64_t const triggerCount;
          uint64_t const eventNumber;
        } const header;
      };
      // Constructor:
      TCDSHeader(const unsigned char* pbytes) :
        tcds_sizes_((tcdssizes*)(pbytes)),
        tcds_header_((tcdsheader*)(pbytes + sizeof(uint64_t))) {}
      // Accessors:
      tcdssizes const& getSizes() const { return *tcds_sizes_; }
      tcdsheader const& getHeader() const { return *tcds_header_; }
      // Extra accessors, in order to avoid code like...
      // myTCDSRecord.getHeader().getHeader().header.macAddress
      // and replace it by...
      // myTCDSRecord.getHeader().getMacAddress()
      unsigned char const& getHeaderSize() const { return tcds_sizes_->size.headerSize; }
      unsigned char const& getSummarySize() const { return tcds_sizes_->size.summarySize; }
      unsigned char const& getL1AhistSize() const { return tcds_sizes_->size.L1AhistSize; }
      unsigned char const& getBSTSize() const { return tcds_sizes_->size.BSTSize; }
      unsigned char const& getBGOSize() const { return tcds_sizes_->size.BGOSize; }
      uint64_t const& getMacAddress() const { return tcds_header_->header.macAddress; }
      uint32_t const& getSw() const { return tcds_header_->header.sw; }
      uint32_t const& getFw() const { return tcds_header_->header.fw; }
      uint32_t const& getReserved0() const { return tcds_header_->header.reserved0; }
      uint32_t const& getFormat() const { return tcds_header_->header.format; }
      uint32_t const& getRunNumber() const { return tcds_header_->header.runNumber; }
      uint32_t const& getReserved1() const { return tcds_header_->header.reserved1; }
      uint32_t const& getActivePartitions2() const { return tcds_header_->header.activePartitions2; }
      uint32_t const& getReserved2() const { return tcds_header_->header.reserved2; }
      uint32_t const& getActivePartitions0() const { return tcds_header_->header.activePartitions0; }
      uint32_t const& getActivePartitions1() const { return tcds_header_->header.activePartitions1; }
      uint32_t const& getNibble() const { return tcds_header_->header.nibble; }
      uint32_t const& getLumiSection() const { return tcds_header_->header.lumiSection; }
      uint16_t const& getNibblesPerLumiSection() const { return tcds_header_->header.nibblesPerLumiSection; }
      uint16_t const& getTriggerTypeFlags() const { return tcds_header_->header.triggerTypeFlags; }
      uint16_t const& getReserved5() const { return tcds_header_->header.reserved5; }
      uint16_t const& getInputs() const { return tcds_header_->header.inputs; }
      uint16_t const& getBcId() const { return tcds_header_->header.bcId; }
      uint16_t const& getOrbitLow() const { return tcds_header_->header.orbitLow; }
      uint32_t const& getOrbitHigh() const { return tcds_header_->header.orbitHigh; }
      uint64_t const& getTriggerCount() const { return tcds_header_->header.triggerCount; }
      uint64_t const& getEventNumber() const { return tcds_header_->header.eventNumber; }

    private:
      tcdssizes* tcds_sizes_;
      tcdsheader* tcds_header_;
    };


    class TCDSL1AHistory {
     public:
      // Source data structure:
      struct history {
        uint16_t const bxId;
        uint16_t const dummy0;
        uint16_t const dummy1;
        unsigned char const dummy2;
        unsigned char const ind0;
        uint32_t const orbitLow;
        uint16_t const orbitHigh;
        unsigned char const eventType;
        unsigned char const ind1;
      };
      union tcdsl1ahistory {
        uint64_t const words[32];
        history const l1aHistory[16];
      };
      // Constructor:
      TCDSL1AHistory(const unsigned char* pbytes) :
        tcds_l1a_history_((tcdsl1ahistory*)(pbytes)) {}
      // Accessor:
      tcdsl1ahistory const& getL1AHistory() const { return *tcds_l1a_history_; }
      // Direct accessor:
      history const* getL1AHistoryRecords() const { return tcds_l1a_history_->l1aHistory; }

    private:
      tcdsl1ahistory* tcds_l1a_history_;
    };


    class TCDSBST {
     public:
      // Source data structure:
      union tcdsbst {
        uint64_t const words[8];
        struct {
          uint32_t const gpsTimeLow;
          uint32_t const gpsTimeHigh;
          uint32_t const bireserved8_11;
          uint32_t const bireserved12_15;
          uint16_t const bstMaster_bireserved16;
          uint16_t const turnCountLow;
          uint16_t const turnCountHigh;
          uint16_t const lhcFillLow;
          uint16_t const lhcFillHigh;
          uint16_t const beamMode;
          uint16_t const particleTypes;
          uint16_t const beamMomentum;
          uint32_t const intensityBeam1;
          uint32_t const intensityBeam2;
          uint32_t const bireserved40_43;
          uint32_t const bireserved44_47;
          uint32_t const bireserved48_51;
          uint32_t const bireserved52_55;
          uint32_t const bireserved56_59;
          uint32_t const bireserved60_63;
        } const bst;
      };
      // Constructor:
      TCDSBST(const unsigned char* pbytes) :
        tcds_bst_((tcdsbst*)(pbytes)) {}
      // Accessor:
      tcdsbst const& getBST() const { return *tcds_bst_; }
      // Direct accessors:
      uint32_t const& getGpsTimeLow() const { return tcds_bst_->bst.gpsTimeLow; }
      uint32_t const& getGpsTimeHigh() const { return tcds_bst_->bst.gpsTimeHigh; }
      uint16_t const& getBstMasterBireserved16() const { return tcds_bst_->bst.bstMaster_bireserved16; }
      uint16_t const& getTurnCountLow() const { return tcds_bst_->bst.turnCountLow; }
      uint16_t const& getTurnCountHigh() const { return tcds_bst_->bst.turnCountHigh; }
      uint16_t const& getLhcFillLow() const { return tcds_bst_->bst.lhcFillLow; }
      uint16_t const& getLhcFillHigh() const { return tcds_bst_->bst.lhcFillHigh; }
      uint16_t const& getBeamMode() const { return tcds_bst_->bst.beamMode; }
      uint16_t const& getParticleTypes() const { return tcds_bst_->bst.particleTypes; }
      uint16_t const& getBeamMomentum() const { return tcds_bst_->bst.beamMomentum; }
      uint32_t const& getIntensityBeam1() const { return tcds_bst_->bst.intensityBeam1; }
      uint32_t const& getIntensityBeam2() const { return tcds_bst_->bst.intensityBeam2; }

    private:
      tcdsbst* tcds_bst_;
    };


    class TCDSRecord{
     public:
      // Constructor:
      TCDSRecord(const unsigned char* pbytes) :
        tcds_fed_header_(pbytes),
        tcds_header_(pbytes +
                     sizeof(fedh_t)),
        tdcs_l1a_history_(pbytes +
                          sizeof(fedh_t) +
                          8 * (tcds_header_.getSizes().size.headerSize + 1)),
        tcds_bst_(pbytes +
                  sizeof(fedh_t) +
                  8 * (tcds_header_.getSizes().size.headerSize + 1) +
                  8 * tcds_header_.getSizes().size.L1AhistSize) {}
      // Accessors:
      TCDSFEDHeader const& getFEDHeader() const { return tcds_fed_header_; }
      TCDSHeader const& getHeader() const { return tcds_header_; }
      TCDSL1AHistory const& getL1AHistory() const { return tdcs_l1a_history_; }
      TCDSBST const& getBST() const { return tcds_bst_; }

     private:
      TCDSFEDHeader tcds_fed_header_;
      TCDSHeader tcds_header_;
      TCDSL1AHistory tdcs_l1a_history_;
      TCDSBST tcds_bst_;
    };

  }
}
#endif
