#include "../test.h"
#include "json-parser/json.h"

#include <cassert>


static void GenStat(Stat* s, const json_value* v) {
    printf("================");
    switch (v->type) {
    case json_object:
        // for (int i = 0; i< v->u.object.length; i++){

        // }
        // for (json_value* child = v->u.object.values->value; child != 0; child = child->parent) {
        //     GenStat(s, child);
        //     ++s->memberCount;
        // }
        // ++s->objectCount;
        s->objectCount = v->u.object.length;
        break;

    case json_array:
        for (int i = 0; i < v->u.array.length; ++i)
            GenStat(s, v->u.array.values[i]);
        s->elementCount += v->u.array.length;
        ++s->arrayCount;
        break;

    case json_string:
        ++s->stringCount;
        s->stringLength += v->u.string.length;
        break;

    case json_integer:
    case json_double:
        ++s->numberCount; 
        break;

    case json_boolean:
        v->u.boolean ? ++s->trueCount: ++s->falseCount;
        break;

    case json_null:
        ++s->nullCount;
        break;

    default:
        assert(false);
    }
}

class JsonParserParseResult : public ParseResultBase {
public:
    JsonParserParseResult() : value() {}
    ~JsonParserParseResult() { json_value_free(value); }

    json_value* value;
};

class JsonParserStringResult : public StringResultBase {
public:
    JsonParserStringResult() : s() {}
    ~JsonParserStringResult() { free(s); }

    virtual const char* c_str() const { return s; }

    char* s;
};

class JsonParserTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "json-parser (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JsonParserParseResult* pr = new JsonParserParseResult;
        pr->value = json_parse(json, length);
    	return pr;
    }
#endif


#ifdef TEST_STRINGIFY
#   undef TEST_STRINGIFY
#endif

#define TEST_STRINGIFY 0
#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JsonParserParseResult* pr = static_cast<const JsonParserParseResult*>(parseResult);
        JsonParserStringResult* sr = new JsonParserStringResult;
        sr->s = cJSON_PrintUnformatted(pr->root);
        return sr;
    }
#endif

#ifdef TEST_PRETTIFY
#   undef TEST_PRETTIFY
#endif

#define TEST_PRETTIFY 0 
#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const JsonParserParseResult* pr = static_cast<const JsonParserParseResult*>(parseResult);
        JusonStringResult* sr = new JusonStringResult;
        sr->s = cJSON_Print(pr->root);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JsonParserParseResult* pr = static_cast<const JsonParserParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->value);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        JsonParserParseResult pr;
        json_value* root = json_parse(json, strlen(json));
        if (root && root->type == json_array && root->u.array.length == 1 &&
            (root->u.array.values[0]->type == json_double)) {
            if(root->u.array.values[0]->type == json_double)
                *d = root->u.array.values[0]->u.dbl;
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        JsonParserParseResult pr;
        json_value* root = json_parse(json, strlen(json));
        if (root && root->type == json_array && root->u.array.length == 1 && root->u.array.values[0]->type == json_string) {
            s = std::string(root->u.array.values[0]->u.string.ptr, root->u.array.values[0]->u.string.length);
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(JsonParserTest);