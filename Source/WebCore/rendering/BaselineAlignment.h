/*
 * Copyright (C) 2023 Apple Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once


namespace WebCore {

class RenderBox;

// These classes are used to implement the Baseline Alignment logic, as described in the CSS Box Alignment
// specification.
// https://drafts.csswg.org/css-align/#baseline-terms
//
// A baseline-sharing group is composed of boxes that participate in baseline alignment together. This is
// possible only if they:
//
//   * Share an alignment context along an axis perpendicular to their baseline alignment axis.
//   * Have compatible baseline alignment preferences (i.e., the baselines that want to align are on the same
//     side of the alignment context).
//
// Once the BaselineGroup is instantiated, defined by a 'block-direction' (WritingMode) and a 'baseline-preference'
// (first/last baseline), it's ready to collect the items that will participate in the Baseline Alignment logic.
//
class BaselineGroup {
public:
    // It stores an item (if not already present) and update the max_ascent associated to this
    // baseline-sharing group.
    void update(const RenderBox&, LayoutUnit ascent);
    LayoutUnit maxAscent() const { return m_maxAscent; }
    int computeSize() const { return m_items.computeSize(); }

private:
    friend class BaselineContext;
    BaselineGroup(WritingMode blockFlow, ItemPosition childPreference);

    // Determines whether a baseline-sharing group is compatible with an item, based on its 'block-flow' and
    // 'baseline-preference'
    bool isCompatible(WritingMode, ItemPosition) const;

    // Determines whether the baseline-sharing group's associated block-flow is opposite (LR vs RL) to particular
    // item's writing-mode.
    bool isOppositeBlockFlow(WritingMode blockFlow) const;

    // Determines whether the baseline-sharing group's associated block-flow is orthogonal (vertical vs horizontal)
    // to particular item's writing-mode.
    bool isOrthogonalBlockFlow(WritingMode blockFlow) const;

    WritingMode m_blockFlow;
    ItemPosition m_preference;
    LayoutUnit m_maxAscent;
    WeakHashSet<const RenderBox> m_items;
};

// https://drafts.csswg.org/css-align-3/#shared-alignment-context
// Boxes share an alignment context along a particular axis when they are:
//
//  * table cells in the same row, along the table's row (inline) axis
//  * table cells in the same column, along the table's column (block) axis
//  * grid items in the same row, along the grid's row (inline) axis
//  * grid items in the same column, along the grid's colum (block) axis
//  * flex items in the same flex line, along the flex container's main axis
//
// https://drafts.csswg.org/css-align-3/#baseline-sharing-group
// A Baseline alignment-context may handle several baseline-sharing groups. In order to create an instance, we
// need to pass the required data to define the first baseline-sharing group; a Baseline Context must have at
// least one baseline-sharing group.
//
// By adding new items to a Baseline Context, the baseline-sharing groups it handles are automatically updated,
// if there is one that is compatible with such item. Otherwise, a new baseline-sharing group is created,
// compatible with the new item.
class BaselineContext {
    WTF_MAKE_FAST_ALLOCATED;
public:
    BaselineContext(const RenderBox& child, ItemPosition preference, LayoutUnit ascent);
    const BaselineGroup& sharedGroup(const RenderBox& child, ItemPosition preference) const;

    // Updates the baseline-sharing group compatible with the item.
    // We pass the item's baseline-preference to avoid dependencies with the LayoutGrid class, which is the one
    // managing the alignment behavior of the Grid Items.
    void updateSharedGroup(const RenderBox& child, ItemPosition preference, LayoutUnit ascent);

private:
    // Returns the baseline-sharing group compatible with an item.
    // We pass the item's baseline-preference to avoid dependencies with the LayoutGrid class, which is the one
    // managing the alignment behavior of the Grid Items.
    // FIXME: Properly implement baseline-group compatibility.
    // See https://github.com/w3c/csswg-drafts/issues/721
    BaselineGroup& findCompatibleSharedGroup(const RenderBox& child, ItemPosition preference);

    Vector<BaselineGroup> m_sharedGroups;
};

} // namespace WebCore
