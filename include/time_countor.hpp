#ifndef __TIMER_HPP__
#define __TIMER_HPP__
#include <iostream>
#include <sys/time.h>
#include <map>

namespace TimeCountor{

    class CTimeCountor {
        public:
            std::map<std::string, std::pair<double, double> > elapsed_time;

            const double get_current_sec() {
                struct timeval tv;
                gettimeofday(&tv,NULL);
                return tv.tv_sec * 1000.0 + tv.tv_usec * 1.0 / 1000;
            }

            void TimerBegin(const std::string& key) 
            {
                if (elapsed_time.find(key) == elapsed_time.end())
                    elapsed_time[key] = std::pair<double, double>(0, 0);

                elapsed_time[key].first = get_current_sec();
            }

            void TimerEnd(const std::string& key) 
            {
                if (elapsed_time.find(key) == elapsed_time.end())
                    elapsed_time[key] = std::pair<double, double>(get_current_sec(), 0);

                elapsed_time[key].second= get_current_sec();
            }

            double ElapsedTime(const std::string& key) 
            {
                if (elapsed_time.find(key) == elapsed_time.end())
                    return 0;

                if (elapsed_time[key].second < elapsed_time[key].first)
                    elapsed_time[key].second = get_current_sec();

                return (elapsed_time[key].second - elapsed_time[key].first);
            }
            std::string ToString (){
                std::string ret;
                for (std::map <std::string, std::pair<double, double> >::iterator it = elapsed_time.begin(); it != elapsed_time.end(); it ++){
                    std::string tmp = it->first + ":";
                    char buf[128];
                    sprintf (buf, "%lf", ElapsedTime (it->first));
                    tmp += buf;
                    if (!ret.length()) {
                        ret += tmp;
                    } else {
                        ret += std::string("\t") + tmp;
                    }
                }
                return ret;
            }
            void clear (){
                elapsed_time.clear ();
            }
    };
}; // namespace TimeCounter

#endif // __TIMER_HPP__
