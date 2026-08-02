#include "scada/standard_reference_types.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

NodeId Std(NumericId numeric_id) {
  return NodeId{numeric_id, 0};
}

TEST(StandardReferenceTypes, ReflexiveMatch) {
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::HasComponent), Std(id::HasComponent)),
            std::optional<bool>{true});
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::References), Std(id::References)),
            std::optional<bool>{true});
}

TEST(StandardReferenceTypes, HasComponentIsAggregateAndHierarchical) {
  // HasComponent -> Aggregates -> HasChild -> HierarchicalReferences ->
  // References.
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::HasComponent), Std(id::Aggregates)),
            std::optional<bool>{true});
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::HasComponent), Std(id::HasChild)),
            std::optional<bool>{true});
  EXPECT_EQ(
      IsStandardReferenceSubtype(Std(id::HasComponent),
                                 Std(id::HierarchicalReferences)),
      std::optional<bool>{true});
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::HasComponent), Std(id::References)),
            std::optional<bool>{true});
}

TEST(StandardReferenceTypes, HasPropertyIsAggregateButNotComponent) {
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::HasProperty), Std(id::Aggregates)),
            std::optional<bool>{true});
  // Siblings under Aggregates are not subtypes of each other.
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::HasProperty), Std(id::HasComponent)),
            std::optional<bool>{false});
}

TEST(StandardReferenceTypes, NonHierarchicalIsNotHierarchical) {
  EXPECT_EQ(
      IsStandardReferenceSubtype(Std(id::HasTypeDefinition),
                                 Std(id::HierarchicalReferences)),
      std::optional<bool>{false});
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::HasTypeDefinition),
                                       Std(id::NonHierarchicalReferences)),
            std::optional<bool>{true});
}

TEST(StandardReferenceTypes, SupertypeIsNotSubtypeOfItsSubtype) {
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::Aggregates), Std(id::HasComponent)),
            std::optional<bool>{false});
  EXPECT_EQ(
      IsStandardReferenceSubtype(Std(id::References), Std(id::HierarchicalReferences)),
      std::optional<bool>{false});
}

TEST(StandardReferenceTypes, StandardSubIsNeverSubtypeOfCustomSuper) {
  // A custom (namespace != 0) super type: a standard reference type never
  // derives from it, so the answer is a definitive false, not nullopt.
  const NodeId custom_super{339, 1};
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::HasComponent), custom_super),
            std::optional<bool>{false});
}

TEST(StandardReferenceTypes, CustomSubIsNotStaticallyKnowable) {
  // A custom (namespace != 0) sub type must be resolved against the server;
  // the predicate declines to answer.
  const NodeId custom_sub{339, 1};
  EXPECT_EQ(IsStandardReferenceSubtype(custom_sub, Std(id::HierarchicalReferences)),
            std::nullopt);
  EXPECT_EQ(IsStandardReferenceSubtype(custom_sub, custom_sub), std::nullopt);
}

TEST(StandardReferenceTypes, UnmodelledStandardIdDeclines) {
  // A namespace-0 numeric id that is not a modelled reference type (here a data
  // type id) is not answerable and must fall back to the address space.
  EXPECT_EQ(IsStandardReferenceSubtype(Std(id::BaseDataType), Std(id::References)),
            std::nullopt);
}

}  // namespace
}  // namespace scada
