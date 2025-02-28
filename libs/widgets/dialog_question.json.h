static const char *dialog_question = "\
{ \
 \"version\": 1, \
 \"widgets\": [ \
  { \
   \"type\": \"frame\", \
   \"uid\": \"title\", \
   \"widget\": { \
    \"rows\": 2, \
    \"widgets\": [ \
     { \
      \"cols\": 2, \
      \"widgets\": [ \
       { \
        \"type\": \"stock\", \
        \"stock\": \"question\" \
       }, \
       { \
        \"type\": \"label\", \
        \"uid\": \"text\" \
       } \
      ] \
     }, \
     { \
      \"cols\": 2, \
      \"halign\": \"fill\", \
      \"cpadf\": \"1, 1, 1\", \
      \"cfill\": \"0, 0\", \
      \"border\": \"none\", \
      \"uniform\": 1, \
      \"widgets\": [ \
       { \
        \"type\": \"button\", \
	\"halign\": \"fill\", \
        \"label\": \"No\", \
	\"btype\": \"cancel\", \
        \"uid\": \"btn_no\" \
       }, \
       { \
        \"type\": \"button\", \
	\"halign\": \"fill\", \
        \"label\": \"Yes\", \
	\"btype\": \"ok\", \
        \"uid\": \"btn_yes\" \
       } \
      ] \
     } \
    ] \
   } \
  } \
 ] \
} \
";
