#ifndef EDITORMAPPER_H
#define EDITORMAPPER_H

#include <QObject>
#include <QStack>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QMetaProperty>
#include <QSet>
#include <QHash>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

using namespace google::protobuf;

class EditorModel;
class BaseEditor;

/**
 * @brief The EditorMapper class used to map widgets into the model.
 * This class provides the data binding functionality allowing not
 * only single indexes into the model to map multiple widgets, but for
 * widgets to be grouped together by context. In other words, this
 * class supports 1:M relationships between QObject properties,
 * including widgets, and model indexes. The organization of the
 * internal source model and protocol buffer is abstracted behind the
 * compile-time generated constants and field numbers of resources.
 */
// NOTE: There is intentionally no API to remove individual mappings
// in this class so that your tiny brain doesn't butcher what is
// otherwise a very elegant execution of basic data structures.
class EditorMapper : public QObject {
  Q_OBJECT

  using ObjectProperty = QPair<QObject*,int>;

  EditorModel* _model; // << editor's tree node sandbox
  QStack<const Descriptor*> _desc; // << field descriptors
  const Descriptor* _rootDesc; // << root descriptor
  QModelIndex _index; // << current mapping index
  QHash<QPersistentModelIndex,QSet<ObjectProperty>> _mappings; // << existing mappings
  QHash<ObjectProperty,QSet<QPersistentModelIndex>> _mappings2; // << existing mappings

 public:
  explicit EditorMapper(EditorModel* model, BaseEditor* parent);

  // the fact this is a QModelIndex is only an implementation detail
  // that you should not rely on at all
  using MapGroup = QModelIndex;

  /**
   * @brief mapField Maps a proto field in the model to an object property.
   * @param fieldNumber The compile-time generated field number constant.
   * @param object The Qt object or widget we want to map.
   * @param property The property name of the object we want to map. The
   *                 default, an empty string, will instead look up the
   *                 USER property on the object, which is the one QWidgets
   *                 are associated with for editing. For example, the
   *                 QLineEdit USER property is the "text" property.
   */
  void mapField(int fieldNumber, QObject *object, const QByteArray& property="");
  /**
   * @brief mapName Convenience for mapping the name field of the resource.
   *
   * This is an abstraction to keep the base editor and subclasses from relying
   * on the structure of tree nodes and knowing about all other resource types
   * which would slow down compilation.
   *
   * @param object The object to map the name to
   * @param property The property name of the object to map. Same as
   *                 mapField parameter.
   */
  void mapName(QObject *object, const QByteArray& property="");
  /**
   * @brief clear Removes all mappings and moves back to the root of the model.
   */
  void clear();
  /**
   * @brief pushField Moves current index into a repeated field.
   *
   * This can be used to map messages within a repeated field and easily
   * remap the entire group later on. It's particular useful for master-detail
   * oriented views like the room editor's background settings which change
   * when the selected background changes.
   *
   * @param fieldNumber The compile-time constant of the repeated field number.
   * @param index The index of the repeated field value, 0 by default.
   * @return MapGroup An index which you can later use to remap the fields.
   */
  MapGroup pushField(int fieldNumber, int index = 0);
  /**
   * @brief pushResource Moves current index into the resource field.
   *
   * Like mapName this is also a convenience function that keeps editors
   * loosely coupled from the structure of the tree node buffer. It's a
   * consequence of the name being in the tree node but not the resource.
   * This also resets the current index back to the root to find the
   * currently set resource of the tree node. That's why there is no
   * popResource, because you can just call this instead!
   */
  void pushResource();
  /**
   * @brief popField Moves current index back to the previous parent.
   */
  void popField();
  /**
   * @brief popRoot Moves back to the root of the model.
   *
   * Existing mappings are not cleared but future mappings will be made relative
   * to the root as they initially were.
   */
  void popRoot();
  void load(const MapGroup& group = QModelIndex(), bool recursive = true);

 signals:
  // these two signals allow us to write to the model or object
  // without the updates becoming cyclic so we can block the
  // update from propagating by blocking our own signals
  modelChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
  objectChanged(ObjectProperty property, const QVariant& value);

 private slots:
  void objectPropertyChanged();
};

#endif  // EDITORMAPPER_H
