#pragma once

#include "scada/event.h"

class ViewEventQueue {
 public:
  using Event =
      std::variant<scada::ModelChangeEvent, scada::SemanticChangeEvent>;

  void AddModelChange(const scada::ModelChangeEvent& event);
  void AddNodeSemanticChange(const scada::SemanticChangeEvent& event);

  std::vector<Event> GetEvents();

 private:
  std::vector<Event> queue_;
};

inline void ViewEventQueue::AddModelChange(
    const scada::ModelChangeEvent& event) {
  for (auto& queued_event : queue_) {
    if (auto* model_change =
            std::get_if<scada::ModelChangeEvent>(&queued_event)) {
      if (model_change->node_id == event.node_id) {
        if (model_change->type_definition_id.is_null())
          model_change->type_definition_id = event.type_definition_id;
        model_change->verb |= event.verb;
        if (model_change->verb & scada::ModelChangeEvent::NodeDeleted)
          model_change->verb = scada::ModelChangeEvent::NodeDeleted;
        return;
      }
    }
  }

  queue_.emplace_back(event);
}

inline void ViewEventQueue::AddNodeSemanticChange(
    const scada::SemanticChangeEvent& event) {
  for (auto& queued_event : queue_) {
    if (auto* semantic_change_event =
            std::get_if<scada::SemanticChangeEvent>(&queued_event)) {
      if (*semantic_change_event == event)
        return;
    }
  }

  queue_.emplace_back(event);
}

inline std::vector<ViewEventQueue::Event> ViewEventQueue::GetEvents() {
  auto queue = std::move(queue_);
  queue_.clear();
  return queue;
}

inline std::ostream& operator<<(std::ostream& stream,
                                const ViewEventQueue::Event& event) {
  if (auto* model_change = std::get_if<scada::ModelChangeEvent>(&event)) {
    stream << *model_change;
  } else if (auto* semantic_change =
                 std::get_if<scada::SemanticChangeEvent>(&event)) {
    stream << *semantic_change;
  } else {
    assert(false);
  }

  return stream;
}
