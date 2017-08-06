#include <json/json.h>
//#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <runtime_graphics.h>

using namespace kext;
using namespace kstd;

extern "C" void json_test()
{
    /*ifstream ifs("/lab4/ui");

    if(!ifs)
    {
        cerr << "json not found\n";
        return;
    }
    else
    {
        //cout << "size: " << ifs.size() << endl;
    }

    char* json = new char[ifs.size()+1];
    ifs.read(json, ifs.size());
    json[ifs.size()] = '\0';
    ifs.close();

    JSONDoc doc;
    //cout << "try parse\n" << json << endl;
    doc.FromString(json);

    cout << "title: " << doc.Obj()["title"].Val().ToString() << endl;*/

    initUI("/lab4/ui");
}

JSONErr *JSONErr::ErrObj = nullptr;

//namespace kext {
//    JSONErr jsonErr;
//}


bool JSONElement::IsArray() const
{

    return value_type == ARRAY;
}

bool JSONElement::IsObject() const
{
    return value_type == OBJECT;
}

bool JSONElement::IsValue() const
{
    return value_type == VALUE;
}

JSONArray &JSONElement::Arr() const
{
    return *(JSONArray*)this;
}

JSONObject &JSONElement::Obj() const
{
    return *(JSONObject*)this;
}

JSONValue &JSONElement::Val() const
{
    return *(JSONValue*)this;
}

bool JSONElement::IsErr() const
{
    return false;
}

JSONElement::~JSONElement()
{

}

bool JSONErr::IsErr() const
{
    return true;
}

JSONValue::JSONValue()
{
    value_type = VALUE;
}

JSONValue::JSONValue(const JSONValue &other)
{
    int len = strlen(other.val)+1;
    val = new char[len];
    strcpy(val, other.val);
    type = other.type;
}

JSONValue &JSONValue::operator=(const JSONValue &other)
{
    delete val;
    int len = strlen(other.val)+1;
    val = new char[len];
    strcpy(val, other.val);
    type = other.type;
    return *this;
}

string JSONValue::ToString() const
{
    return val;
}

int JSONValue::ToInt() const
{
    return atoi(val);
}

double JSONValue::ToDouble() const
{
    return atof(val);
}

bool JSONValue::ToBool() const
{
    return strcmp(val, "false");
}

bool JSONValue::IsString() const
{
    return type == STRING;
}

bool JSONValue::IsInt() const
{
    return type == INT;
}

JSONValue::~JSONValue()
{
    //cout << "~JSONValue(" << val << ")" << endl;
    delete val;
}

JSONObject::JSONObject()
{
    value_type = OBJECT;
}

JSONElement &JSONObject::operator[](const string &key) const
{
    for(JSONElement* e : elements)
    {
        if(e->key == key)
            return *e;
    }
    return *JSONErr::ErrObj;
}

JSONElement &JSONObject::operator[](const char *key) const
{
    for(JSONElement* e : elements)
    {
        if(e->key == key)
            return *e;
    }
    return *JSONErr::ErrObj;
}

bool JSONObject::exists(const string &key) const
{
    return !(*this)[key].IsErr();
}

JSONObject::~JSONObject()
{
    for(JSONElement* elem : elements)
    {
        //cout << "~JSONObject()" << endl;
        delete elem;
    }
}

JSONArray::JSONArray()
{
    value_type = ARRAY;
}

JSONArray::JSONArray(const JSONArray &other)
{
    elements = other.elements;
    key = other.key;
}

JSONArray &JSONArray::operator=(const JSONArray &other)
{
    elements = other.elements;
    key = other.key;
    return *this;
}

JSONElement &JSONArray::operator[](uint idx) const
{
    if(idx >= elements.size())
        return *JSONErr::ErrObj;
    return *elements[idx];
}

bool JSONArray::exists(uint idx) const
{
    return !(*this)[idx].IsErr();
}

JSONElement *JSONArray::front() const
{
    return elements[0];
}

JSONElement *JSONArray::back() const
{
    return elements.back();
}

vector<JSONElement*>::iterator JSONArray::begin() const
{
    return elements.begin();
}

vector<JSONElement*>::iterator JSONArray::end() const
{
    return elements.end();
}

uint JSONArray::size() const
{
    return elements.size();
}

JSONArray::~JSONArray()
{
    for(JSONElement* elem : elements)
    {
        //cout << "~JSONArray() {";
        delete elem;
        //cout << "}" << endl;
    }
}

JSONDoc::JSONDoc()
{
    if(JSONErr::ErrObj == nullptr)
        JSONErr::ErrObj = new JSONErr;
}

void JSONDoc::FromString(const string &src)
{
    const char* raw = src.c_str();
    uint pos = 0;
    //string token = "";

    //cout << "sk\n";
    SK();

    if(raw[pos] == '{')
    {
        ++pos;
        //cout << "p:obj\n";
        root = parseObject(raw, pos);
    }
    else if(raw[pos] == '[')
    {
        ++pos;
        //cout << "p:arr\n";
        root = parseArray(raw, pos);
    }
}

JSONArray &JSONDoc::Arr() const
{
    return root->Arr();
}

JSONObject &JSONDoc::Obj() const
{
    return root->Obj();
}

bool JSONDoc::IsObject() const
{
    return root->IsObject();
}

bool JSONDoc::IsArray() const
{
    return root->IsArray();
}

JSONDoc::~JSONDoc()
{
    //root->~JSONElement();
    //cout << "~JSONDoc()" << endl;
    delete root;
}

JSONObject *JSONDoc::parseObject(const char *raw, uint &pos)
{
    JSONObject *obj = new JSONObject;

    //SK();

    /*
        if(raw[pos] == '{')
        {
            ++pos;
            parseObject(raw, pos, obj);
        }
        else if(raw[pos] == '[')
        {
            ++pos;
            parseArray(raw, pos, obj);
        }
        */
    while(raw[pos] != '}')
    {
        SK();
        if(raw[pos] == '"')
        {
            uint cur = ++pos;

            while(raw[pos] != '"')
                ++pos;

            char* str = new char[pos-cur+1];
            memcpy(str, raw+cur, pos-cur);
            str[pos-cur] = '\0';
            ++pos;

            SK();

            if(raw[pos] != ':')
            {
                ;
            }
            ++pos;
            //cout << "p:val: " << str << endl;
            JSONElement* elem = parseValue(raw, pos);
            elem->key = str;
            obj->elements.push_back(elem);
        }
        if(raw[pos] == '}')
        {
            ++pos;
            break;
        }
        if(raw[pos] != ',')
        {
            break;
        }
        ++pos;
    }

    return obj;
}

JSONArray *JSONDoc::parseArray(const char *raw, uint &pos)
{
    JSONArray *arr = new JSONArray;

    SK();

    /*
        if(raw[pos] == '{')
        {
            ++pos;
            parseObject(raw, pos, obj);
        }
        else if(raw[pos] == '[')
        {
            ++pos;
            parseArray(raw, pos, obj);
        }
        */
    while(raw[pos] != ']')
    {
        SK();
        //cout << "pa" << endl;
        arr->elements.push_back(parseValue(raw, pos));
        if(raw[pos] == ']')
        {
            ++pos;
            break;
        }
        if(raw[pos] != ',')
        {
            break;
        }
        ++pos;
    }
    SK();

    return arr;
}

JSONElement *JSONDoc::parseValue(const char *raw, uint &pos)
{
    JSONElement *obj;// = new JSONObject;

    SK();

    if(raw[pos] == '"')
    {
        //cout << "pv:str: ";
        JSONValue *val = new JSONValue;
        val->type = JSONValue::STRING;

        uint cur = ++pos;

        while(raw[pos] != '"')
            ++pos;

        val->val = new char[pos-cur+1];
        memcpy(val->val, raw+cur, pos-cur);
        val->val[pos-cur] = '\0';
        //cout << val->val << endl;
        ++pos;

        obj = val;
    }
    else if(isdigit(raw[pos]) || (raw[pos] == '-' && isdigit(raw[pos+1])))
    {
        //cout << "pv:num: ";
        uint cur = pos;
        while(isdigit(raw[++pos]))
            ;
        //--pos;

        JSONValue *val = new JSONValue;
        val->type = JSONValue::INT;

        val->val = new char[pos-cur+1];
        memcpy(val->val, raw+cur, pos-cur);
        val->val[pos-cur] = '\0';
        //cout << val->val << endl;
        ++pos;

        obj = val;
    }
    else if(raw[pos] == '{')
    {
        ++pos;
        //cout << "pv:obj\n";
        obj = parseObject(raw, pos);
    }
    else if(raw[pos] == '[')
    {
        ++pos;
        //cout << "pv:arr\n";
        obj = parseArray(raw, pos);
    }
    SK();
    return obj;
}

void JSONDoc::skipSpaces(const char *raw, uint &pos)
{
    while(raw[pos] == ' ' || raw[pos] == '\t' || raw[pos] == '\r' || raw[pos] == '\n')
        ++pos;
}
