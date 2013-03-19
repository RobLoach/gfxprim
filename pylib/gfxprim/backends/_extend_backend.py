from ..utils import extend, add_swig_getmethod, add_swig_setmethod
from . import c_backends

def extend_backend(_backend):
  """
  Extends _backend class with convenience methods.
  Called once on module inicialization.
  """

  @extend(_backend, name='__str__')
  @extend(_backend, name='__repr__')
  def backend_str(self):
    return "<Backend \"%s\", GP_Backend %sowned>" % (
      self.name, "" if self.thisown else "not ")

  @extend(_backend)
  def Flip(self):
    "If display is buffered, this copies content of context onto display."
    return c_backends.GP_BackendFlip(self)

  @extend(_backend)
  def UpdateRect(self, rect):
    "Update a rectangle on a buffered backend."
    return c_backends.GP_BackendUpdateRect(self, rect[0], rect[1], rect[2], rect[3])

  @extend(_backend)
  def Poll(self):
    "Poll the backend for events."
    return c_backends.GP_BackendPoll(self)

  @extend(_backend)
  def SetCaption(self, caption):
    "Set backend window caption (if possible)"
    return c_backends.GP_BackendSetCaption(self, caption)

  @extend(_backend)
  def Resize(self, w, h):
    "Resize backend window (if possible)"
    return c_backends.GP_BackendResize(self, w, h)
