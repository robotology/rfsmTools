classdef StateMachine < SwigRef
  methods
    function this = swig_this(self)
      this = rfsmMEX(3, self);
    end
    function self = StateMachine(varargin)
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = rfsmMEX(57, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
    function delete(self)
      if self.swigPtr
        rfsmMEX(58, self);
        self.swigPtr=[];
      end
    end
    function varargout = getFileName(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(59, self, varargin{:});
    end
    function varargout = load(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(60, self, varargin{:});
    end
    function varargout = run(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(61, self, varargin{:});
    end
    function varargout = step(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(62, self, varargin{:});
    end
    function varargout = sendEvent(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(63, self, varargin{:});
    end
    function varargout = sendEvents(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(64, self, varargin{:});
    end
    function varargout = doString(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(65, self, varargin{:});
    end
    function varargout = doFile(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(66, self, varargin{:});
    end
    function varargout = addLuaPackagePath(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(67, self, varargin{:});
    end
    function varargout = close(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(68, self, varargin{:});
    end
    function varargout = setStateCallback(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(69, self, varargin{:});
    end
    function varargout = getCurrentState(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(70, self, varargin{:});
    end
    function varargout = getEventsList(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(71, self, varargin{:});
    end
    function varargout = getEventQueue(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(72, self, varargin{:});
    end
    function varargout = getStateGraph(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(73, self, varargin{:});
    end
    function varargout = enablePreStepHook(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(74, self, varargin{:});
    end
    function varargout = enablePostStepHook(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(75, self, varargin{:});
    end
    function varargout = onPreStep(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(76, self, varargin{:});
    end
    function varargout = onPostStep(self,varargin)
      [varargout{1:nargout}] = rfsmMEX(77, self, varargin{:});
    end
  end
  methods(Static)
  end
end
