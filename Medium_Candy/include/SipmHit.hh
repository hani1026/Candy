    #ifndef SIPM_HIT_HH
    #define SIPM_HIT_HH

    #include <G4VHit.hh>
    #include <G4THitsCollection.hh>
    #include <vector>
    #include <numeric>

    /** 1 개의 SiPM(Window)에서, 한 이벤트 동안 시간‑bin별 광자 수를 저장 */
    class SipmHit : public G4VHit
    {
    public:
        /* ---- 시간 파라미터 ---- */
        static constexpr double kBinWidthNs = 16.0;   //!< bin 폭 16 ns
        static constexpr double kWindowNs   = 4096.0;  //!< 분석 창 0‑4096 ns
        static constexpr int    kMaxBins    =
            static_cast<int>(kWindowNs / kBinWidthNs);   //!< 13 bin (0‑12)

        explicit SipmHit(int id = -1);
        ~SipmHit() override = default;

        /** t (ns)를 16 ns bin에 반영 */
        void Fill(double t_ns);

        /* ── 게터 ── */
        int                       GetID()   const { return fID; }
        const std::vector<int>&   GetHist() const { return fHist; }
        int                       GetCount() const
        { return std::accumulate(fHist.begin(), fHist.end(), 0); }

    private:
        int              fID;
        std::vector<int> fHist;   //!< size = kMaxBins
    };

    /* Geant4 히트 컬렉션 타입 */
    using SipmHitsCollection = G4THitsCollection<SipmHit>;

    #endif // SIPM_HIT_HH
