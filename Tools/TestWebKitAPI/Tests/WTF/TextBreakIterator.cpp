/*
 * Copyright (C) 2016-2023 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include <wtf/text/TextBreakIterator.h>

namespace TestWebKitAPI {

static String makeUTF16(std::vector<UChar> input)
{
    return { input.data(), static_cast<unsigned>(input.size()) };
}

TEST(WTF, TextBreakIteratorNumGraphemeClusters)
{
    EXPECT_EQ(0U, numGraphemeClusters(StringView { }));
    EXPECT_EQ(0U, numGraphemeClusters(StringView { ""_s }));
    EXPECT_EQ(0U, numGraphemeClusters(makeUTF16({ })));

    EXPECT_EQ(1U, numGraphemeClusters(StringView { "a"_s }));
    EXPECT_EQ(1U, numGraphemeClusters(makeUTF16({ 'a' })));
    EXPECT_EQ(1U, numGraphemeClusters(StringView { "\r\n"_s }));
    EXPECT_EQ(1U, numGraphemeClusters(StringView { "\n"_s }));
    EXPECT_EQ(1U, numGraphemeClusters(StringView { "\r"_s }));
    EXPECT_EQ(1U, numGraphemeClusters(makeUTF16({ '\r', '\n' })));
    EXPECT_EQ(1U, numGraphemeClusters(makeUTF16({ '\n' })));
    EXPECT_EQ(1U, numGraphemeClusters(makeUTF16({ '\r' })));

    EXPECT_EQ(2U, numGraphemeClusters(StringView { "\n\r"_s }));
    EXPECT_EQ(2U, numGraphemeClusters(makeUTF16({ '\n', '\r' })));

    EXPECT_EQ(2U, numGraphemeClusters(StringView { "\r\n\r"_s }));
    EXPECT_EQ(2U, numGraphemeClusters(makeUTF16({ '\r', '\n', '\r' })));

    EXPECT_EQ(1U, numGraphemeClusters(makeUTF16({ 'g', 0x308 })));
    EXPECT_EQ(1U, numGraphemeClusters(makeUTF16({ 0x1100, 0x1161, 0x11A8 })));
    EXPECT_EQ(1U, numGraphemeClusters(makeUTF16({ 0x0BA8, 0x0BBF })));

    EXPECT_EQ(2U, numGraphemeClusters(makeUTF16({ 0x308, 'g' })));

    EXPECT_EQ(3U, numGraphemeClusters(StringView { "\r\nbc"_s }));
    EXPECT_EQ(3U, numGraphemeClusters(makeUTF16({ 'g', 0x308, 'b', 'c' })));
}

TEST(WTF, TextBreakIteratorNumCodeUnitsInGraphemeClusters)
{
    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(StringView { }, 0));
    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(StringView { }, 1));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(StringView { ""_s }, 0));
    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(StringView { ""_s }, 1));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(makeUTF16({ }), 0));
    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(makeUTF16({ }), 1));

    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(StringView { "a"_s }, 1));
    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'a' }), 1));
    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(StringView { "\n"_s }, 1));
    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(StringView { "\r"_s }, 1));
    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\n' }), 1));
    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\r' }), 1));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(StringView { "abc"_s }, 0));
    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(StringView { "abc"_s }, 1));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(StringView { "abc"_s }, 2));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(StringView { "abc"_s }, 3));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(StringView { "abc"_s }, 4));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'a', 'b', 'c' }), 0));
    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'a', 'b', 'c' }), 1));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'a', 'b', 'c' }), 2));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'a', 'b', 'c' }), 3));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'a', 'b', 'c' }), 4));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(StringView { "\r\n"_s }, 0));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(StringView { "\r\n"_s }, 1));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(StringView { "\r\n"_s }, 2));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(StringView { "\r\n"_s }, 3));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\r', '\n' }), 0));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\r', '\n' }), 1));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\r', '\n' }), 2));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\r', '\n' }), 3));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(StringView { "\n\r"_s }, 0));
    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(StringView { "\n\r"_s }, 1));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(StringView { "\n\r"_s }, 2));

    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\n', '\r' }), 1));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\n', '\r' }), 2));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(StringView { "\r\n\r"_s }, 0));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(StringView { "\r\n\r"_s }, 1));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(StringView { "\r\n\r"_s }, 2));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(StringView { "\r\n\r"_s }, 3));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\r', '\n', '\r' }), 0));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\r', '\n', '\r' }), 1));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\r', '\n', '\r' }), 2));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(makeUTF16({ '\r', '\n', '\r' }), 3));

    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'g', 0x308 }), 1));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(makeUTF16({ 0x1100, 0x1161, 0x11A8 }), 1));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(makeUTF16({ 0x0BA8, 0x0BBF }), 1));

    EXPECT_EQ(1U, numCodeUnitsInGraphemeClusters(makeUTF16({ 0x308, 'g' }), 1));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(StringView { "\r\nbc"_s }, 0));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(StringView { "\r\nbc"_s }, 1));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(StringView { "\r\nbc"_s }, 2));
    EXPECT_EQ(4U, numCodeUnitsInGraphemeClusters(StringView { "\r\nbc"_s }, 3));
    EXPECT_EQ(4U, numCodeUnitsInGraphemeClusters(StringView { "\r\nbc"_s }, 4));
    EXPECT_EQ(4U, numCodeUnitsInGraphemeClusters(StringView { "\r\nbc"_s }, 5));

    EXPECT_EQ(0U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'g', 0x308, 'b', 'c' }), 0));
    EXPECT_EQ(2U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'g', 0x308, 'b', 'c' }), 1));
    EXPECT_EQ(3U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'g', 0x308, 'b', 'c' }), 2));
    EXPECT_EQ(4U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'g', 0x308, 'b', 'c' }), 3));
    EXPECT_EQ(4U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'g', 0x308, 'b', 'c' }), 4));
    EXPECT_EQ(4U, numCodeUnitsInGraphemeClusters(makeUTF16({ 'g', 0x308, 'b', 'c' }), 5));
}

TEST(WTF, TextBreakIteratorBehaviors)
{
    {
        auto string = u"ぁぁ"_str;
        auto locale = AtomString("ja"_str);
        CachedTextBreakIterator strictIterator({ string }, TextBreakIterator::LineMode { TextBreakIterator::LineMode::Behavior::Strict }, locale);
        EXPECT_EQ(strictIterator.following(0), 2);
        CachedTextBreakIterator looseIterator({ string }, TextBreakIterator::LineMode { TextBreakIterator::LineMode::Behavior::Loose }, locale);
        EXPECT_EQ(looseIterator.following(0), 1);
    }
    {
        auto string = u"中〜"_str;
        auto locale = AtomString("zh-Hans"_str);
        CachedTextBreakIterator strictIterator({ string }, TextBreakIterator::LineMode { TextBreakIterator::LineMode::Behavior::Strict }, locale);
        EXPECT_EQ(strictIterator.following(0), 2);
        CachedTextBreakIterator looseIterator({ string }, TextBreakIterator::LineMode { TextBreakIterator::LineMode::Behavior::Loose }, locale);
        EXPECT_EQ(looseIterator.following(0), 1);
    }
}

} // namespace TestWebKitAPI
