classdef MatlabSwigIterator < SwigRef
  methods
    function this = swig_this(self)
      this = rfsmMEX(3, self);
    end
    function delete(self)
      if self.swigPtr
        rfsmMEX(5, self);
        self.swigPtr=[];
      end
    end
    function varargout = value(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(6, self, varargin{:});
    end
    function varargout = incr(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(7, self, varargin{:});
    end
    function varargout = decr(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(8, self, varargin{:});
    end
    function varargout = distance(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(9, self, varargin{:});
    end
    function varargout = equal(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(10, self, varargin{:});
    end
    function varargout = copy(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(11, self, varargin{:});
    end
    function varargout = next(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(12, self, varargin{:});
    end
    function varargout = previous(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(13, self, varargin{:});
    end
    function varargout = advance(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(14, self, varargin{:});
    end
    function varargout = eq(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(15, self, varargin{:});
    end
    function varargout = ne(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(16, self, varargin{:});
    end
    function varargout = TODOincr(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(17, self, varargin{:});
    end
    function varargout = TODOdecr(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(18, self, varargin{:});
    end
    function varargout = plus(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(19, self, varargin{:});
    end
    function varargout = minus(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(20, self, varargin{:});
    end
    function self = MatlabSwigIterator(varargin)
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        error('No matching constructor');
      end
    end
  end
  methods(Static)
  end
end
