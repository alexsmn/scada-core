#pragma once

namespace scada {

struct ModelChangeEvent;
struct SemanticChangeEvent;

class ViewEvents {
 public:
  virtual ~ViewEvents() {}

  virtual void OnModelChanged(const ModelChangeEvent& event) = 0;
  virtual void OnNodeSemanticsChanged(const SemanticChangeEvent& event) = 0;
};

}  // namespace scada
