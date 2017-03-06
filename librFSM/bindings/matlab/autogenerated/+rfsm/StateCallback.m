classdef StateCallback < SwigRef
  methods
    function this = swig_this(self)
      this = rfsmMEX(3, self);
    end
    function delete(self)
      if self.swigPtr
        rfsmMEX(46, self);
        self.swigPtr=[];
      end
    end
    function varargout = entry(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(47, self, varargin{:});
    end
    function varargout = doo(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(48, self, varargin{:});
    end
    function varargout = exit(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(49, self, varargin{:});
    end
    function self = StateCallback(varargin)
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = rfsmMEX(50, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
  end
  methods(Static)
  end
end
