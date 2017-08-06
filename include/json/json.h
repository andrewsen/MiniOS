#ifndef JSON_H
#define JSON_H

#include <vector>
#include <string>
#include <cstring>

namespace kext {

using namespace kstd;

class JSONArray;
class JSONObject;
class JSONValue;

class JSONElement
{
protected:
    enum ValueType
    {
        VALUE, ARRAY, OBJECT
    } value_type;
    string key;
    //JSONElement *value;

    friend class JSONDoc;
    friend class JSONObject;
public:

    bool IsArray() const;

    bool IsObject() const;

    bool IsValue() const;

    JSONArray& Arr() const;

    JSONObject& Obj() const;

    JSONValue& Val() const;

    virtual bool IsErr() const;

    virtual ~JSONElement();
};

class JSONErr : public JSONElement
{
public:
    static JSONErr *ErrObj;

    virtual bool IsErr() const;
};

class JSONValue : public JSONElement
{
    enum Type
    {
        STRING, INT, DOUBLE, BOOL
    } type;

    friend class JSONDoc;

protected:
    char* val;

public:
    JSONValue();

    JSONValue(const JSONValue &other);

    JSONValue& operator=(const JSONValue &other);

    string  ToString() const;

    int ToInt() const;

    double ToDouble() const;

    bool ToBool() const;

    bool IsString() const;

    bool IsInt() const;

    virtual ~JSONValue();
};

class JSONObject : public JSONElement
{
    vector<JSONElement*> elements;

    friend class JSONDoc;
public:
    JSONObject();

    JSONElement& operator[](const string &key) const;

    JSONElement& operator[](const char *key) const;

    bool exists(const string &key) const;

    virtual ~JSONObject();
};

class JSONArray : public JSONElement
{
    vector<JSONElement*> elements;

    friend class JSONDoc;
public:
    JSONArray();

    JSONArray(const JSONArray &other);

    JSONArray& operator=(const JSONArray &other);

    JSONElement& operator[](uint idx) const;

    bool exists(uint idx) const;
    JSONElement* front() const;

    JSONElement* back() const;

    vector<JSONElement*>::iterator begin() const;

    vector<JSONElement*>::iterator end() const;

    uint size() const;

    virtual ~JSONArray();
};

#define SK() skipSpaces(raw, pos)

class JSONDoc
{
    JSONElement *root;

public:
    JSONDoc();

    void FromString(const string &src);

    JSONArray& Arr() const;

    JSONObject& Obj() const;

    bool IsObject() const;

    bool IsArray() const;

    virtual ~JSONDoc();

private:

    JSONObject* parseObject(const char *raw, uint &pos);

    JSONArray* parseArray(const char *raw, uint &pos);

    JSONElement *parseValue(const char *raw, uint &pos);

    void skipSpaces(const char *raw, uint &pos);
};

//extern JSONErr jsonErr;

}
//#undef SK
#endif // JSON_H
