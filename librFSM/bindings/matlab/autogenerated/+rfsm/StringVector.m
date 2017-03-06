classdef StringVector < SwigRef
  methods
    function this = swig_this(self)
      this = rfsmMEX(3, self);
    end
    function varargout = pop(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(21, self, varargin{:});
    end
    function varargout = brace(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(22, self, varargin{:});
    end
    function varargout = setbrace(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(23, self, varargin{:});
    end
    function varargout = append(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(24, self, varargin{:});
    end
    function varargout = empty(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(25, self, varargin{:});
    end
    function varargout = size(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(26, self, varargin{:});
    end
    function varargout = swap(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(27, self, varargin{:});
    end
    function varargout = begin(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(28, self, varargin{:});
    end
    function varargout = end(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(29, self, varargin{:});
    end
    function varargout = rbegin(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(30, self, varargin{:});
    end
    function varargout = rend(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(31, self, varargin{:});
    end
    function varargout = clear(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(32, self, varargin{:});
    end
    function varargout = get_allocator(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(33, self, varargin{:});
    end
    function varargout = pop_back(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(34, self, varargin{:});
    end
    function varargout = erase(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(35, self, varargin{:});
    end
    function self = StringVector(varargin)
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = rfsmMEX(36, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
    function varargout = push_back(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(37, self, varargin{:});
    end
    function varargout = front(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(38, self, varargin{:});
    end
    function varargout = back(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(39, self, varargin{:});
    end
    function varargout = assign(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(40, self, varargin{:});
    end
    function varargout = resize(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(41, self, varargin{:});
    end
    function varargout = insert(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(42, self, varargin{:});
    end
    function varargout = reserve(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(43, self, varargin{:});
    end
    function varargout = capacity(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(44, self, varargin{:});
    end
    function delete(self)
      if self.swigPtr
        rfsmMEX(45, self);
        self.swigPtr=[];
      end
    end
  end
  methods(Static)
  end
end
