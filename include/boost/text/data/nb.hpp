// Copyright (C) 2020 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Warning! This file is autogenerated.
#ifndef BOOST_TEXT_DATA_NB_HPP
#define BOOST_TEXT_DATA_NB_HPP

#include <boost/text/string_view.hpp>


namespace boost { namespace text { namespace data { namespace nb {

inline string_view search_collation_tailoring()
{
    return string_view((char const *)
u8R"(  
  
[normalization on]
[suppressContractions [เ-ไ ເ-ໄ ꪵ ꪶ ꪹ ꪻ ꪼ\u19B5-\u19B7\u19BA]]
&'='<'≠'
&ا
  <<<ﺎ<<<ﺍ
  <<آ
  <<<ﺂ<<<ﺁ
  <<أ
  <<<ﺄ<<<ﺃ
  <<إ
  <<<ﺈ<<<ﺇ
&و
  <<<ۥ
  <<<ﻮ<<<ﻭ
  <<ؤ
  <<<ﺆ<<<ﺅ
&ي
  <<<ۦ
  <<<ﻳ<<<ﻴ<<<ﻲ<<<ﻱ
  <<ئ
  <<<ﺋ<<<ﺌ<<<ﺊ<<<ﺉ
  <<ى
  <<<ﯨ<<<ﯩ
  <<<ﻰ<<<ﻯ
&ه
  <<<ﻫ<<<ﻬ<<<ﻪ<<<ﻩ
  <<ة
  <<<ﺔ<<<ﺓ
&[last primary ignorable]<<׳
  <<״
  <<ـ
  <<ฺ
&ᄀ
  =ᆨ
&ᄀᄀ
  =ᄁ=ᆩ
&ᄀᄉ
  =ᆪ
&ᄂ
  =ᆫ
&ᄂᄌ
  =ᆬ
&ᄂᄒ
  =ᆭ
&ᄃ
  =ᆮ
&ᄃᄃ
  =ᄄ
&ᄅ
  =ᆯ
&ᄅᄀ
  =ᆰ
&ᄅᄆ
  =ᆱ
&ᄅᄇ
  =ᆲ
&ᄅᄉ
  =ᆳ
&ᄅᄐ
  =ᆴ
&ᄅᄑ
  =ᆵ
&ᄅᄒ
  =ᆶ
&ᄆ
  =ᆷ
&ᄇ
  =ᆸ
&ᄇᄇ
  =ᄈ
&ᄇᄉ
  =ᆹ
&ᄉ
  =ᆺ
&ᄉᄉ
  =ᄊ=ᆻ
&ᄋ
  =ᆼ
&ᄌ
  =ᆽ
&ᄌᄌ
  =ᄍ
&ᄎ
  =ᆾ
&ᄏ
  =ᆿ
&ᄐ
  =ᇀ
&ᄑ
  =ᇁ
&ᄒ
  =ᇂ
&ᅡᅵ
  =ᅢ
&ᅣᅵ
  =ᅤ
&ᅥᅵ
  =ᅦ
&ᅧᅵ
  =ᅨ
&ᅩᅡ
  =ᅪ
&ᅩᅡᅵ
  =ᅫ
&ᅩᅵ
  =ᅬ
&ᅮᅥ
  =ᅯ
&ᅮᅥᅵ
  =ᅰ
&ᅮᅵ
  =ᅱ
&ᅳᅵ
  =ᅴ
  
  
&D<<đ<<<Đ<<ð<<<Ð
&t<<<þ/h
&T<<<Þ/H
&Y<<ü<<<Ü<<ű<<<Ű
&[before 1]ǀ<æ<<<Æ<<ä<<<Ä<<ę<<<Ę<ø<<<Ø<<ö<<<Ö<<ő<<<Ő<<œ<<<Œ<å<<<Å<<aa<<<Aa<<<AA
  
  )");
}

inline string_view standard_collation_tailoring()
{
    return string_view((char const *)
u8R"(  
&D<<đ<<<Đ<<ð<<<Ð
&t<<<þ/h
&T<<<Þ/H
&Y<<ü<<<Ü<<ű<<<Ű
&[before 1]ǀ<æ<<<Æ<<ä<<<Ä<<ę<<<Ę<ø<<<Ø<<ö<<<Ö<<ő<<<Ő<<œ<<<Œ<å<<<Å<<aa<<<Aa<<<AA
  )");
}


}}}}

#endif
