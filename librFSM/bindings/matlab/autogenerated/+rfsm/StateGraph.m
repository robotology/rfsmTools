classdef StateGraph < SwigRef
  methods
    function this = swig_this(self)
      this = rfsmMEX(3, self);
    end
    function varargout = states(self, varargin)
      narginchk(1, 2)
      if nargin==1
        nargoutchk(0, 1)
        varargout{1} = rfsmMEX(51, self);
      else
        nargoutchk(0, 0)
        rfsmMEX(52, self, varargin{1});
      end
    end
    function varargout = transitions(self, varargin)
      narginchk(1, 2)
      if nargin==1
        nargoutchk(0, 1)
        varargout{1} = rfsmMEX(53, self);
      else
        nargoutchk(0, 0)
        rfsmMEX(54, self, varargin{1});
      end
    end
    function self = StateGraph(varargin)
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = rfsmMEX(55, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
    function delete(self)
      if self.swigPtr
        rfsmMEX(56, self);
        self.swigPtr=[];
      end
    end
  end
  methods(Static)
  end
end
