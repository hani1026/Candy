    #include "SipmHit.hh"

    /* Geant4 NEW_DELETE 매크로용 allocator */
    G4ThreadLocal G4Allocator<SipmHit>* SipmHitAllocator = nullptr;

    SipmHit::SipmHit(int id)
        : fID(id), fHist(kMaxBins, 0) {}

    void SipmHit::Fill(double t_ns)
    {
        int bin = static_cast<int>(t_ns / kBinWidthNs);
        if (bin >= 0 && bin < kMaxBins)
            ++fHist[bin];                 // 해당 16 ns bin 증가
    }
