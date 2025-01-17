(function()
{
 var Global=this,Runtime=this.IntelliFactory.Runtime,Html,Client,Attribute,Pagelet,Default,Implementation,Element,Enumerator,Math,document,jQuery,Events,JQueryEventSupport,AttributeBuilder,DeprecatedTagBuilder,JQueryHtmlProvider,TagBuilder,Text,EventsPervasives;
 Runtime.Define(Global,{
  WebSharper:{
   Html:{
    Client:{
     Attribute:Runtime.Class({
      get_Body:function()
      {
       var attr;
       attr=this.HtmlProvider.CreateAttribute(this.Name);
       attr.value=this.Value;
       return attr;
      }
     },{
      New:function(htmlProvider,name,value)
      {
       var a;
       a=Attribute.New1(htmlProvider);
       a.Name=name;
       a.Value=value;
       return a;
      },
      New1:function(HtmlProvider)
      {
       var r;
       r=Runtime.New(this,Pagelet.New());
       r.HtmlProvider=HtmlProvider;
       return r;
      }
     }),
     AttributeBuilder:Runtime.Class({
      Class:function(x)
      {
       return this.NewAttr("class",x);
      },
      NewAttr:function(name,value)
      {
       var a;
       a=Attribute.New(this.HtmlProvider,name,value);
       return a;
      }
     },{
      New:function(HtmlProvider)
      {
       var r;
       r=Runtime.New(this,{});
       r.HtmlProvider=HtmlProvider;
       return r;
      }
     }),
     Default:{
      A:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("a",x);
      },
      Action:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("action",x);
      },
      Align:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("align",x);
      },
      Alt:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("alt",x);
      },
      Attr:Runtime.Field(function()
      {
       return Implementation.Attr();
      }),
      B:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("b",x);
      },
      Body:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("body",x);
      },
      Br:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("br",x);
      },
      Button:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("button",x);
      },
      Code:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("code",x);
      },
      Deprecated:Runtime.Field(function()
      {
       return Implementation.DeprecatedHtml();
      }),
      Div:function(x)
      {
       return Default.Tags().Div(x);
      },
      Em:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("em",x);
      },
      Form:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("form",x);
      },
      H1:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("h1",x);
      },
      H2:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("h2",x);
      },
      H3:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("h3",x);
      },
      H4:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("h4",x);
      },
      HRef:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("href",x);
      },
      Head:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("head",x);
      },
      Height:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("height",x);
      },
      Hr:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("hr",x);
      },
      I:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("i",x);
      },
      IFrame:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("iframe",x);
      },
      Id:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("id",x);
      },
      Img:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("img",x);
      },
      Input:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("input",x);
      },
      LI:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("li",x);
      },
      Name:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("name",x);
      },
      NewAttr:function(x)
      {
       return function(arg10)
       {
        return Default.Attr().NewAttr(x,arg10);
       };
      },
      OL:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("ol",x);
      },
      OnLoad:function(init)
      {
       return Implementation.HtmlProvider().OnDocumentReady(init);
      },
      P:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("p",x);
      },
      Pre:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("pre",x);
      },
      RowSpan:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("rowspan",x);
      },
      Script:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("script",x);
      },
      Select:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("select",x);
      },
      Selected:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("selected",x);
      },
      Span:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("span",x);
      },
      Src:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("src",x);
      },
      TBody:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("tbody",x);
      },
      TD:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("td",x);
      },
      TFoot:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("tfoot",x);
      },
      TH:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("th",x);
      },
      THead:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("thead",x);
      },
      TR:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("tr",x);
      },
      Table:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("table",x);
      },
      Tags:Runtime.Field(function()
      {
       return Implementation.Tags();
      }),
      Text:function(x)
      {
       return Default.Tags().text(x);
      },
      TextArea:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("textarea",x);
      },
      UL:function(x)
      {
       var _this;
       _this=Default.Tags();
       return _this.NewTag("ul",x);
      },
      VAlign:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("valign",x);
      },
      Width:function(x)
      {
       var _this;
       _this=Default.Attr();
       return _this.NewAttr("width",x);
      }
     },
     DeprecatedAttributeBuilder:Runtime.Class({
      NewAttr:function(name,value)
      {
       var a;
       a=Attribute.New(this.HtmlProvider,name,value);
       return a;
      }
     },{
      New:function(HtmlProvider)
      {
       var r;
       r=Runtime.New(this,{});
       r.HtmlProvider=HtmlProvider;
       return r;
      }
     }),
     DeprecatedTagBuilder:Runtime.Class({
      NewTag:function(name,children)
      {
       var el,enumerator,pl;
       el=Element.New(this.HtmlProvider,name);
       enumerator=Enumerator.Get(children);
       while(enumerator.MoveNext())
        {
         pl=enumerator.get_Current();
         el.AppendI(pl);
        }
       return el;
      }
     },{
      New:function(HtmlProvider)
      {
       var r;
       r=Runtime.New(this,{});
       r.HtmlProvider=HtmlProvider;
       return r;
      }
     }),
     Element:Runtime.Class({
      AppendI:function(pl)
      {
       var body,_,objectArg,arg00,objectArg1,arg001,arg10,_1,r;
       body=pl.get_Body();
       if(body.nodeType===2)
        {
         objectArg=this["HtmlProvider@33"];
         arg00=this.get_Body();
         _=objectArg.AppendAttribute(arg00,body);
        }
       else
        {
         objectArg1=this["HtmlProvider@33"];
         arg001=this.get_Body();
         arg10=pl.get_Body();
         _=objectArg1.AppendNode(arg001,arg10);
        }
       if(this.IsRendered)
        {
         _1=pl.Render();
        }
       else
        {
         r=this.RenderInternal;
         _1=void(this.RenderInternal=function()
         {
          r(null);
          return pl.Render();
         });
        }
       return _1;
      },
      AppendN:function(node)
      {
       var objectArg,arg00;
       objectArg=this["HtmlProvider@33"];
       arg00=this.get_Body();
       return objectArg.AppendNode(arg00,node);
      },
      OnLoad:function(f)
      {
       var objectArg,arg00;
       objectArg=this["HtmlProvider@33"];
       arg00=this.get_Body();
       return objectArg.OnLoad(arg00,f);
      },
      Render:function()
      {
       var _;
       if(!this.IsRendered)
        {
         this.RenderInternal.call(null,null);
         _=void(this.IsRendered=true);
        }
       else
        {
         _=null;
        }
       return _;
      },
      get_Body:function()
      {
       return this.Dom;
      },
      get_Html:function()
      {
       return this["HtmlProvider@33"].GetHtml(this.get_Body());
      },
      get_HtmlProvider:function()
      {
       return this["HtmlProvider@33"];
      },
      get_Id:function()
      {
       var objectArg,arg00,id,_,newId,objectArg1,arg001;
       objectArg=this["HtmlProvider@33"];
       arg00=this.get_Body();
       id=objectArg.GetProperty(arg00,"id");
       if(id===undefined?true:id==="")
        {
         newId="id"+Math.round(Math.random()*100000000);
         objectArg1=this["HtmlProvider@33"];
         arg001=this.get_Body();
         objectArg1.SetProperty(arg001,"id",newId);
         _=newId;
        }
       else
        {
         _=id;
        }
       return _;
      },
      get_Item:function(name)
      {
       var objectArg,arg00,objectArg1,arg001;
       objectArg=this["HtmlProvider@33"];
       arg00=this.get_Body();
       objectArg.GetAttribute(arg00,name);
       objectArg1=this["HtmlProvider@33"];
       arg001=this.get_Body();
       return objectArg1.GetAttribute(arg001,name);
      },
      get_Text:function()
      {
       return this["HtmlProvider@33"].GetText(this.get_Body());
      },
      get_Value:function()
      {
       return this["HtmlProvider@33"].GetValue(this.get_Body());
      },
      set_Html:function(x)
      {
       var objectArg,arg00;
       objectArg=this["HtmlProvider@33"];
       arg00=this.get_Body();
       return objectArg.SetHtml(arg00,x);
      },
      set_Item:function(name,value)
      {
       var objectArg,arg00;
       objectArg=this["HtmlProvider@33"];
       arg00=this.get_Body();
       return objectArg.SetAttribute(arg00,name,value);
      },
      set_Text:function(x)
      {
       var objectArg,arg00;
       objectArg=this["HtmlProvider@33"];
       arg00=this.get_Body();
       return objectArg.SetText(arg00,x);
      },
      set_Value:function(x)
      {
       var objectArg,arg00;
       objectArg=this["HtmlProvider@33"];
       arg00=this.get_Body();
       return objectArg.SetValue(arg00,x);
      }
     },{
      New:function(html,name)
      {
       var el,dom;
       el=Element.New1(html);
       dom=document.createElement(name);
       el.RenderInternal=function()
       {
       };
       el.Dom=dom;
       el.IsRendered=false;
       return el;
      },
      New1:function(HtmlProvider)
      {
       var r;
       r=Runtime.New(this,Pagelet.New());
       r["HtmlProvider@33"]=HtmlProvider;
       return r;
      }
     }),
     Events:{
      JQueryEventSupport:Runtime.Class({
       OnBlur:function(f,el)
       {
        return jQuery(el.get_Body()).bind("blur",function()
        {
         return f(el);
        });
       },
       OnChange:function(f,el)
       {
        return jQuery(el.get_Body()).bind("change",function()
        {
         return f(el);
        });
       },
       OnClick:function(f,el)
       {
        return this.OnMouse("click",f,el);
       },
       OnDoubleClick:function(f,el)
       {
        return this.OnMouse("dblclick",f,el);
       },
       OnError:function(f,el)
       {
        return jQuery(el.get_Body()).bind("error",function()
        {
         return f(el);
        });
       },
       OnFocus:function(f,el)
       {
        return jQuery(el.get_Body()).bind("focus",function()
        {
         return f(el);
        });
       },
       OnKeyDown:function(f,el)
       {
        return jQuery(el.get_Body()).bind("keydown",function(ev)
        {
         return(f(el))({
          KeyCode:ev.keyCode
         });
        });
       },
       OnKeyPress:function(f,el)
       {
        return jQuery(el.get_Body()).keypress(function(arg)
        {
         return(f(el))({
          CharacterCode:arg.which
         });
        });
       },
       OnKeyUp:function(f,el)
       {
        return jQuery(el.get_Body()).bind("keyup",function(ev)
        {
         return(f(el))({
          KeyCode:ev.keyCode
         });
        });
       },
       OnLoad:function(f,el)
       {
        return jQuery(el.get_Body()).bind("load",function()
        {
         return f(el);
        });
       },
       OnMouse:function(name,f,el)
       {
        return jQuery(el.get_Body()).bind(name,function(ev)
        {
         return(f(el))({
          X:ev.pageX,
          Y:ev.pageY
         });
        });
       },
       OnMouseDown:function(f,el)
       {
        return this.OnMouse("mousedown",f,el);
       },
       OnMouseEnter:function(f,el)
       {
        return this.OnMouse("mouseenter",f,el);
       },
       OnMouseLeave:function(f,el)
       {
        return this.OnMouse("mouseleave",f,el);
       },
       OnMouseMove:function(f,el)
       {
        return this.OnMouse("mousemove",f,el);
       },
       OnMouseOut:function(f,el)
       {
        return this.OnMouse("mouseout",f,el);
       },
       OnMouseUp:function(f,el)
       {
        return this.OnMouse("mouseup",f,el);
       },
       OnResize:function(f,el)
       {
        return jQuery(el.get_Body()).bind("resize",function()
        {
         return f(el);
        });
       },
       OnScroll:function(f,el)
       {
        return jQuery(el.get_Body()).bind("scroll",function()
        {
         return f(el);
        });
       },
       OnSelect:function(f,el)
       {
        return jQuery(el.get_Body()).bind("select",function()
        {
         return f(el);
        });
       },
       OnSubmit:function(f,el)
       {
        return jQuery(el.get_Body()).bind("submit",function()
        {
         return f(el);
        });
       },
       OnUnLoad:function(f,el)
       {
        return jQuery(el.get_Body()).bind("unload",function()
        {
         return f(el);
        });
       }
      },{
       New:function()
       {
        return Runtime.New(this,{});
       }
      })
     },
     EventsPervasives:{
      Events:Runtime.Field(function()
      {
       return JQueryEventSupport.New();
      })
     },
     Implementation:{
      Attr:Runtime.Field(function()
      {
       return AttributeBuilder.New(Implementation.HtmlProvider());
      }),
      DeprecatedHtml:Runtime.Field(function()
      {
       return DeprecatedTagBuilder.New(Implementation.HtmlProvider());
      }),
      HtmlProvider:Runtime.Field(function()
      {
       return JQueryHtmlProvider.New();
      }),
      JQueryHtmlProvider:Runtime.Class({
       AddClass:function(node,cls)
       {
        return jQuery(node).addClass(cls);
       },
       AppendAttribute:function(node,attr)
       {
        var arg10,arg20;
        arg10=attr.nodeName;
        arg20=attr.value;
        return this.SetAttribute(node,arg10,arg20);
       },
       AppendNode:function(node,el)
       {
        return jQuery(node).append(jQuery(el));
       },
       Clear:function(node)
       {
        return jQuery(node).contents().detach();
       },
       CreateAttribute:function(str)
       {
        return document.createAttribute(str);
       },
       CreateElement:function(name)
       {
        return document.createElement(name);
       },
       CreateTextNode:function(str)
       {
        return document.createTextNode(str);
       },
       GetAttribute:function(node,name)
       {
        return jQuery(node).attr(name);
       },
       GetHtml:function(node)
       {
        return jQuery(node).html();
       },
       GetProperty:function(node,name)
       {
        var x;
        x=jQuery(node).attr(name);
        return x;
       },
       GetText:function(node)
       {
        return node.textContent;
       },
       GetValue:function(node)
       {
        var x;
        x=jQuery(node).val();
        return x;
       },
       HasAttribute:function(node,name)
       {
        return jQuery(node).attr(name)!=null;
       },
       OnDocumentReady:function(f)
       {
        return jQuery(document).ready(f);
       },
       OnLoad:function(node,f)
       {
        return jQuery(node).ready(f);
       },
       Remove:function(node)
       {
        return jQuery(node).remove();
       },
       RemoveAttribute:function(node,name)
       {
        return jQuery(node).removeAttr(name);
       },
       RemoveClass:function(node,cls)
       {
        return jQuery(node).removeClass(cls);
       },
       SetAttribute:function(node,name,value)
       {
        return jQuery(node).attr(name,value);
       },
       SetCss:function(node,name,prop)
       {
        return jQuery(node).css(name,prop);
       },
       SetHtml:function(node,text)
       {
        return jQuery(node).html(text);
       },
       SetProperty:function(node,name,value)
       {
        var x;
        x=jQuery(node).prop(name,value);
        return x;
       },
       SetStyle:function(node,style)
       {
        return jQuery(node).attr("style",style);
       },
       SetText:function(node,text)
       {
        node.textContent=text;
       },
       SetValue:function(node,value)
       {
        return jQuery(node).val(value);
       }
      },{
       New:function()
       {
        return Runtime.New(this,{});
       }
      }),
      Tags:Runtime.Field(function()
      {
       return TagBuilder.New(Implementation.HtmlProvider());
      })
     },
     Operators:{
      OnAfterRender:function(f,w)
      {
       var r;
       r=w.Render;
       w.Render=function()
       {
        r.apply(w);
        return f(w);
       };
       return;
      },
      OnBeforeRender:function(f,w)
      {
       var r;
       r=w.Render;
       w.Render=function()
       {
        f(w);
        return r.apply(w);
       };
       return;
      },
      add:function(el,inner)
      {
       var enumerator,pl;
       enumerator=Enumerator.Get(inner);
       while(enumerator.MoveNext())
        {
         pl=enumerator.get_Current();
         el.AppendI(pl);
        }
       return el;
      }
     },
     Pagelet:Runtime.Class({
      AppendTo:function(targetId)
      {
       var target,value;
       target=document.getElementById(targetId);
       value=target.appendChild(this.get_Body());
       return this.Render();
      },
      Render:function()
      {
       return null;
      },
      ReplaceInDom:function(node)
      {
       var value;
       value=node.parentNode.replaceChild(this.get_Body(),node);
       return this.Render();
      }
     },{
      New:function()
      {
       return Runtime.New(this,{});
      }
     }),
     TagBuilder:Runtime.Class({
      Div:function(x)
      {
       return this.NewTag("div",x);
      },
      NewTag:function(name,children)
      {
       var el,enumerator,pl;
       el=Element.New(this.HtmlProvider,name);
       enumerator=Enumerator.Get(children);
       while(enumerator.MoveNext())
        {
         pl=enumerator.get_Current();
         el.AppendI(pl);
        }
       return el;
      },
      text:function(data)
      {
       return Text.New(data);
      }
     },{
      New:function(HtmlProvider)
      {
       var r;
       r=Runtime.New(this,{});
       r.HtmlProvider=HtmlProvider;
       return r;
      }
     }),
     Text:Runtime.Class({
      get_Body:function()
      {
       return document.createTextNode(this.text);
      }
     },{
      New:function(text)
      {
       var r;
       r=Runtime.New(this,Pagelet.New());
       r.text=text;
       return r;
      }
     })
    }
   }
  }
 });
 Runtime.OnInit(function()
 {
  Html=Runtime.Safe(Global.WebSharper.Html);
  Client=Runtime.Safe(Html.Client);
  Attribute=Runtime.Safe(Client.Attribute);
  Pagelet=Runtime.Safe(Client.Pagelet);
  Default=Runtime.Safe(Client.Default);
  Implementation=Runtime.Safe(Client.Implementation);
  Element=Runtime.Safe(Client.Element);
  Enumerator=Runtime.Safe(Global.WebSharper.Enumerator);
  Math=Runtime.Safe(Global.Math);
  document=Runtime.Safe(Global.document);
  jQuery=Runtime.Safe(Global.jQuery);
  Events=Runtime.Safe(Client.Events);
  JQueryEventSupport=Runtime.Safe(Events.JQueryEventSupport);
  AttributeBuilder=Runtime.Safe(Client.AttributeBuilder);
  DeprecatedTagBuilder=Runtime.Safe(Client.DeprecatedTagBuilder);
  JQueryHtmlProvider=Runtime.Safe(Implementation.JQueryHtmlProvider);
  TagBuilder=Runtime.Safe(Client.TagBuilder);
  Text=Runtime.Safe(Client.Text);
  return EventsPervasives=Runtime.Safe(Client.EventsPervasives);
 });
 Runtime.OnLoad(function()
 {
  Runtime.Inherit(Attribute,Pagelet);
  Runtime.Inherit(Element,Pagelet);
  Runtime.Inherit(Text,Pagelet);
  Implementation.Tags();
  Implementation.HtmlProvider();
  Implementation.DeprecatedHtml();
  Implementation.Attr();
  EventsPervasives.Events();
  Default.Tags();
  Default.Deprecated();
  Default.Attr();
  return;
 });
}());
