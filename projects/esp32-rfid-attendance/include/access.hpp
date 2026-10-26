#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>
namespace access_demo {
enum class Decision { granted, denied, duplicate, busy, invalid };
inline const char* label(Decision d){switch(d){case Decision::granted:return "granted";case Decision::denied:return "denied";case Decision::duplicate:return "duplicate";case Decision::busy:return "busy";default:return "invalid";}}
struct Event {uint32_t ms;Decision decision;const char* subject;bool unlocked;};
class Controller {
  bool unlocked_=false,seen_=false;
  uint32_t opened_=0,lastScan_=0;
  char lastUid_[21]{};
  static bool valid(const char* uid){
    size_t n=std::strlen(uid);if(n!=8&&n!=14&&n!=20)return false;
    for(size_t i=0;i<n;++i)if(!((uid[i]>='0'&&uid[i]<='9')||(uid[i]>='A'&&uid[i]<='F')))return false;
    return true;
  }
public:
  bool tick(uint32_t now){if(unlocked_&&uint32_t(now-opened_)>=3000)unlocked_=false;return unlocked_;}
  Event scan(uint32_t now,const char* uid){
    tick(now);
    if(!valid(uid))return {now,Decision::invalid,"unknown",unlocked_};
    if(seen_&&std::strcmp(lastUid_,uid)==0&&uint32_t(now-lastScan_)<2000)
      return {now,Decision::duplicate,"unknown",unlocked_};
    std::strcpy(lastUid_,uid);lastScan_=now;seen_=true;
    if(unlocked_)return {now,Decision::busy,"unknown",true};
    // Invented demonstration credentials, never real student identifiers.
    const char* person=std::strcmp(uid,"A1B2C3D4")==0 ? "demo_user_01" :
      std::strcmp(uid,"11223344")==0 ? "demo_user_02" : nullptr;
    if(!person)return {now,Decision::denied,"unknown",false};
    unlocked_=true;opened_=now;return {now,Decision::granted,person,true};
  }
};
inline int json(Event e,bool simulated,char* out,size_t len){
 return std::snprintf(out,len,"{\"uptime_ms\":%u,\"decision\":\"%s\",\"subject\":\"%s\",\"relay_unlocked\":%s,\"simulated\":%s}",unsigned(e.ms),label(e.decision),e.subject,e.unlocked?"true":"false",simulated?"true":"false");
}
}
