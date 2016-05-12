#ifndef _TTREESOURCE_H_
#define _TTREESOURCE_H_

#ifndef __CINT__
#   include <mutex>
#endif

#include <vector>

#include "TObject.h"
#include "TRawEvent.h"
#include "TRawSource.h"
#include "TChain.h"
#include "TTree.h"

templateClassImp(TTreeSource)

template <typename T>
class TTreeSource : public TRawEventSource {
public:
  TTreeSource() {;}

  TTreeSource(const char* filename, const char* treename, const char* eventclassname, kFileType file_type)
    : TTreeSource<T>({filename},treename,eventclassname,file_type) { ; }

  template<typename... Args>
  TTreeSource(const char* filename, const char* treename, const char* eventclassname, kFileType file_type, Args&&... args)
    : TTreeSource<T>({filename},treename,eventclassname,file_type,std::forward<Args>(args)...) { ; }

  template<typename... Args>
  TTreeSource(const std::vector<char*>& filenames, const char* treename, const char* eventclassname, kFileType file_type, Args&&... args)
    : fChain(treename), fCurrentEntry(0) {

    assert(file_type == kFileType::ROOT_TTREE);
    fFileType = file_type;


    for (auto fn : filenames) {
      fChain.Add(fn);
    }
    fFileSize =  fChain.GetEntries()*sizeof(T);

    if (sizeof...(Args) > 0) {
      fEvent = new T(std::forward<Args>(args)...);
    } else {
      fEvent = new T();
    }
    fChain.SetBranchAddress(eventclassname, &fEvent);
  }

  ~TTreeSource() {;}

  virtual std::string Status() const {return std::string("");}
  virtual std::string SourceDescription() const {return std::string("");}


  kFileType GetFileType() const { return fFileType; }
  long GetFileSize() const { return fFileSize; }

  virtual void Reset() {;}

protected:
  void SetFileSize(long file_size) { fFileSize = file_size; }

private:
  virtual int GetEvent(TRawEvent& event) {
    fEvent = new T(*fEvent); // copy construct a new object from the old
    fChain.GetEntry(fCurrentEntry); // fill the new object with current event data

    // save the pointer of this object into the TRawEvents SmartBuffer

    fCurrentEntry++;
    //fEvent = nullptr;
    return sizeof(*fEvent);
  }

  TChain fChain;
  kFileType fFileType;
  long fFileSize;
  T* fEvent;
  long fCurrentEntry;

  ClassDef(TTreeSource,0);
};



#endif
