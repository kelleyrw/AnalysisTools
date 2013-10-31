#ifndef CNumBase_h
#define CNumBase_h
  
template <class numtype=float> 
class CNumBase {
    protected:
    private:
        numtype data_;
        numtype stat_; //should I make it so that the errors can be different above and below?
        numtype syst_;
    public:
        CNumBase(numtype datan=0.0, numtype statn=0.0, numtype systn=0.0);
        CNumBase(const CNumBase<numtype>& obj);
        numtype data()  const;
        numtype stat()  const;
        numtype syst()  const;
        numtype error() const;
        void print(bool statn=true, bool systn=true) const;
        CNumBase<numtype> setData(numtype datan);
        CNumBase<numtype> setStat(numtype statn);
        CNumBase<numtype> setSyst(numtype systn);
        CNumBase<numtype> gaussian();
        CNumBase<numtype> operator*(numtype scale) const;
        CNumBase<numtype> operator*(const CNumBase<numtype>& mul) const;
        CNumBase<numtype> operator/(numtype scale) const;
        CNumBase<numtype> operator/(const CNumBase<numtype>& div) const;
        CNumBase<numtype> operator+(const CNumBase<numtype>& add) const;
        CNumBase<numtype> operator-(const CNumBase<numtype>& sub) const;
        CNumBase<numtype>& operator=(const CNumBase<numtype>& equal);
        //still need to overload the boolean operators for comparison
        //do we want to make comparisons include the errors
        //ie (4+/-1 == 3.5+/-.4) would return true?
};

// include the template definitions
#include "AnalysisTools/LanguageTools/src/CNumBase.impl.h"

#endif // #define CNumBase_h
