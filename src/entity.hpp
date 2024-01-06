#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

class entity;

class component {
  // Component manages the memory inside each entity chunk - the entity's
  // accessor calls the component's methods.
public:
  void *get(const entity &pEntity) const;
  void set(entity &pEntity, void *pValue);
  void remove(entity &pEntity);

  void handle_init(entity &pEntity);
  void handle_destroy(entity &pEntity);

  // Each entity chunk contains the memory spaces that entity's components can
  // use. This specifies the memory size used by the component for each entity.
  int memory_size();
};

class entity_handle {
public:
  int id() const;
  int version() const;

private:
  int mId;
  int mVersion;
};

class entity {
public:
  void get_component_memory(void **pPosition);
  const entity_handle &handle() const;

private:
  entity_handle mHandle;
  bool mDeleted;
  bool mFloating;
};

class entity_chunk {
public:
  void get_component_memory(void **pPosition, int *pStride);
};

class entity_group {};

class entity_query {};

class entity_store {};

#endif // __ENTITY_HPP__
