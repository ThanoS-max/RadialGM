#include "Models/ModelMapper.h"

#include "Editors/BaseEditor.h"

ModelMapper::ModelMapper(MessageModel *model, BaseEditor *parent) : QObject(parent), _model(model) {
  _mapper = new ImmediateDataWidgetMapper(this);
  _mapper->setOrientation(Qt::Vertical);
  _mapper->setModel(model);
  parent->connect(model, &ProtoModel::DataChanged, parent, &BaseEditor::dataChanged);
}

// mapper

MessageModel *ModelMapper::GetModel() { return _model; }

void ModelMapper::addMapping(QWidget *widget, int section, QByteArray propName) {
  _mapper->addMapping(widget, section, propName);
}

void ModelMapper::clearMapping() { _mapper->clearMapping(); }

void ModelMapper::toFirst() { _mapper->toFirst(); }

void ModelMapper::setOrientation(Qt::Orientation aOrientation) {
  _mapper->setOrientation(aOrientation);
}

void ModelMapper::setCurrentIndex(int index) {
  _mapper->setCurrentIndex(index);
}

void ModelMapper::setModel(MessageModel *model) {
  _mapper->setModel(model);
  _model = model;
}

void ModelMapper::revert() {
  _mapper->revert();
}

// model

void ModelMapper::ReplaceBuffer(google::protobuf::Message *buffer) { _model->ReplaceBuffer(buffer); }

bool ModelMapper::RestoreBackup() { return _model->RestoreBackup(); }

void ModelMapper::SetDirty(bool dirty) { _model->SetDirty(dirty); }

bool ModelMapper::IsDirty() { return _model->IsDirty(); }
