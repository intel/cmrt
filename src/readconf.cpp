/* Guo Yejun yejun.guo@intel.com*/

#include "readconf.h"
#include <map>
#include <string>
#include <fstream>
#include <string.h>

class ReadConf
{
public:
    ReadConf()
    {
        LoadDefault();
        ReadFromFile();
    }
    ~ReadConf()
    {
        confs.clear();
    }
    std::string GetConf(std::string key)
    {
        return confs[key];
    }
private:
    void LoadDefault()
    {
        confs.clear();
        confs["jitter"] = "igfxcmjit";
    }
    char* RemoveHeadingSpace(char* line)
    {
        int i = 0;
        while (line[i] != '\0')
        {
            if (line[i] != ' ' && line[i] != '\t')
            {
                break;
            }
            i++;
        }
        return &line[i];
    }
    void RemoveTailingSpace(char* line)
    {
        for (int i = strlen(line); i >= 0; ++i)
        {
            if (line[i] != ' ' && line[i] != '\t')
            {
                line[i+1] = '\0';
                break;
            }
        }
    }
    void ReadFromFile()
    {
        const char* filename = SYSCONFDIR"/cmrt.conf";

        std::ifstream file(filename);
        char line[1024];
        while (file.getline(line, 1024))
        {
            char* head = RemoveHeadingSpace(line);
            if (head[0] == '#' || head[0] == '\0')
            {
                continue;
            }
            RemoveTailingSpace(head);
            char* key = strtok(head, "= \t");
            char* value = strtok(NULL, "= \t");
            confs[key] = value;
        }
    }
    std::map<std::string, std::string> confs;
};

static ReadConf conf;

const char* GetJitterName()
{
    static std::string base;

    if (base.empty())
    {
        base = conf.GetConf("jitter");
        if (sizeof(void *) == 4)
        {
            base += "32.so";
        }
        else
        {
            base += "64.so";
        }
    }

    return base.c_str();
}
