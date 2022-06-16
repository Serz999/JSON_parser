#include "gtest/gtest.h"
#include "JSON_library/JSON_parser.h"

using namespace std::string_literals;

TEST(JSON_parser, parsing_test_1) {
    std::stringstream input_stream;
    input_stream << "{\"glossary1\":1,\"glossary2\":2345}";
    Document doc_real = Load(input_stream);
    Document doc_expected(Dict{{"glossary1",1},{"glossary2",2345}});
    ASSERT_EQ(doc_real, doc_expected);
}

TEST(JSON_parser, parsing_test_2) {
    std::stringstream input_stream;
    input_stream << "{\"glossary1\":1.11,\"glossary2\":2345.00}";
    Document doc_real = Load(input_stream);
    Document doc_expected(Dict{{"glossary1",1.11},{"glossary2",2345}});
    ASSERT_EQ(doc_real, doc_expected);
}

TEST(JSON_parser, parsing_test_3) {
    std::stringstream input_stream;
    input_stream << "{\"glossary3\":true,\"glossary4\":false}";
    Document doc_real = Load(input_stream);
    Document doc_expected(Dict{{"glossary3",true},{"glossary4",false}});
    ASSERT_EQ(doc_real, doc_expected);
}

TEST(JSON_parser, parsing_test_4) {
    std::stringstream input_stream;
    input_stream << "{\"glossary3\":null,\"glossary4\":null}";
    Document doc_real = Load(input_stream);
    Document doc_expected(Dict{{"glossary3",nullptr},{"glossary4", nullptr}});
    ASSERT_EQ(doc_real, doc_expected);
}

TEST(JSON_parser, parsing_test_5) {
    std::stringstream input_stream;
    input_stream << "{\"glossary\":[1,2,3,4,5]}";
    Document doc_real = Load(input_stream);
    Document doc_expected(Dict{{"glossary",Array{1,2,3,4,5}}});
    ASSERT_EQ(doc_real, doc_expected);
}

TEST(JSON_parser, parsing_test_6) {
    std::stringstream input_stream;
    input_stream << "{\"glossary1\":{\"inner_glossary1\":1,\"inner_glossary2\":2},\"ID\":100}";
    Document doc_real = Load(input_stream);
    Document doc_expected(Dict{{"glossary1",Dict{{"inner_glossary1",1},{"inner_glossary2",2}}},{"ID",100}});
    ASSERT_EQ(doc_real, doc_expected);
}

TEST(JSON_parser, parsing_test_escape_symbols_reading) {
    std::stringstream input_stream;
    input_stream << "{\n\t\"glossary1\":{\n\t\r\"inner_glossary1\":1,\n\t\"inner_glossary2\":2\n},\n\t\"ID\":100}";
    Document doc_real = Load(input_stream);
    Document doc_expected(Dict{{"glossary1",Dict{{"inner_glossary1",1},{"inner_glossary2",2}}},{"ID",100}});
    ASSERT_EQ(doc_real, doc_expected);
}

TEST(JSON_parser, parsing_test_complex_1) {
    std::stringstream input_stream;
    input_stream << "{\"array\":[[1,2],[[1,2],[1,2]]]}";
    Document doc_real = Load(input_stream);
    Document doc_expected(Dict{{"array",Array{Array{1,2},Array{Array{1,2},Array{1,2}}}}});
    ASSERT_EQ(doc_real, doc_expected);
}

TEST(JSON_parser, parsing_test_complex_2) {
    std::stringstream input_stream;
    input_stream << "{\"glossary1\":1,\"glossary2\":2,\"glossary3\":true,\"glossary4\":false,\"glossary5\":{\"inner_glossary1\":12.29,\"inner_glossary2\":12.00,\"inner_glossary3\":null,\"inner_glossary4\":[1,2,3,4,5]}}";
    Document doc_real = Load(input_stream);
    Document doc_expected(Dict{{"glossary1",1},{"glossary2",2},{"glossary3",true},{"glossary4",false},{"glossary5",{Dict{{"inner_glossary1",12.29},{"inner_glossary2",12.00},{"inner_glossary3",nullptr},{"inner_glossary4",Array{1,2,3,4,5}}}}}});
    ASSERT_EQ(doc_real, doc_expected);
}

TEST(JSON_parser, parsing_test_print) {
    Document example_doc(Dict{{"glossary1",1},{"glossary2",2345}});
    std::stringstream output_stream_real;
    Print(example_doc, output_stream_real);
    std::stringstream output_stream_expected;
    output_stream_expected << "{\n\t\"glossary1\": 1,\n\t\"glossary2\": 2345\t\n}";
    ASSERT_EQ(output_stream_real.str(), output_stream_expected.str());
}