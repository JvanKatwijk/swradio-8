<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>1072</width>
    <height>610</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>swradio-8</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <widget class="QwtPlot" name="hfSpectrum">
    <property name="geometry">
     <rect>
      <x>40</x>
      <y>30</y>
      <width>1001</width>
      <height>241</height>
     </rect>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Main Spectrum Window. This window shows the spectrum of the signal arriving from the selected input device, decimated to the selected inputrate.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
   </widget>
   <widget class="QSlider" name="hfScopeSlider">
    <property name="geometry">
     <rect>
      <x>20</x>
      <y>40</y>
      <width>24</width>
      <height>171</height>
     </rect>
    </property>
    <property name="orientation">
     <enum>Qt::Vertical</enum>
    </property>
   </widget>
   <widget class="QwtPlot" name="lfSpectrum">
    <property name="geometry">
     <rect>
      <x>300</x>
      <y>290</y>
      <width>741</width>
      <height>241</height>
     </rect>
    </property>
   </widget>
   <widget class="QSlider" name="lfScopeSlider">
    <property name="geometry">
     <rect>
      <x>270</x>
      <y>340</y>
      <width>21</width>
      <height>141</height>
     </rect>
    </property>
    <property name="orientation">
     <enum>Qt::Vertical</enum>
    </property>
   </widget>
   <widget class="QComboBox" name="decoderTable">
    <property name="geometry">
     <rect>
      <x>160</x>
      <y>0</y>
      <width>141</width>
      <height>31</height>
     </rect>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;decoder selector. The decoders listed here are part of the configured implementation.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
    <item>
     <property name="text">
      <string>select decoder</string>
     </property>
    </item>
   </widget>
   <widget class="QLCDNumber" name="frequencyDisplay">
    <property name="geometry">
     <rect>
      <x>100</x>
      <y>40</y>
      <width>241</width>
      <height>61</height>
     </rect>
    </property>
    <property name="font">
     <font>
      <pointsize>13</pointsize>
     </font>
    </property>
    <property name="frameShape">
     <enum>QFrame::NoFrame</enum>
    </property>
    <property name="lineWidth">
     <number>4</number>
    </property>
    <property name="digitCount">
     <number>11</number>
    </property>
    <property name="segmentStyle">
     <enum>QLCDNumber::Flat</enum>
    </property>
   </widget>
   <widget class="QPushButton" name="freqButton">
    <property name="geometry">
     <rect>
      <x>30</x>
      <y>550</y>
      <width>111</width>
      <height>21</height>
     </rect>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Touching this button will cause a widget to appear with a keypad on which a frequency - in kHz or mHz - can be specified. Use the kHz or mHz button on this pad to acknowledge the choice.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
    <property name="text">
     <string>select freq</string>
    </property>
   </widget>
   <widget class="QComboBox" name="bandSelector">
    <property name="geometry">
     <rect>
      <x>300</x>
      <y>0</y>
      <width>111</width>
      <height>31</height>
     </rect>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;band selector. One may choose among several precoded bandwidth settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
    <item>
     <property name="text">
      <string>am</string>
     </property>
    </item>
    <item>
     <property name="text">
      <string>usb</string>
     </property>
    </item>
    <item>
     <property name="text">
      <string>lsb</string>
     </property>
    </item>
    <item>
     <property name="text">
      <string>wide</string>
     </property>
    </item>
   </widget>
   <widget class="QComboBox" name="streamOutSelector">
    <property name="geometry">
     <rect>
      <x>620</x>
      <y>0</y>
      <width>151</width>
      <height>31</height>
     </rect>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;select an audio output device. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
   </widget>
   <widget class="QPushButton" name="freqSave">
    <property name="geometry">
     <rect>
      <x>250</x>
      <y>550</y>
      <width>111</width>
      <height>21</height>
     </rect>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The selected frequency can be saved. Touching this button will show a small widget where you&lt;/p&gt;&lt;p&gt;can give the frequency a name as label.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
    <property name="text">
     <string>frequency save</string>
    </property>
   </widget>
   <widget class="QPushButton" name="middleButton">
    <property name="geometry">
     <rect>
      <x>140</x>
      <y>550</y>
      <width>111</width>
      <height>21</height>
     </rect>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Touching this button will change the oscillator such that the selected frequency will be in the middke&lt;/p&gt;&lt;p&gt;of the right half of the frequency display.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
    <property name="text">
     <string>middle</string>
    </property>
   </widget>
   <widget class="QSpinBox" name="mouse_Inc">
    <property name="geometry">
     <rect>
      <x>520</x>
      <y>0</y>
      <width>101</width>
      <height>31</height>
     </rect>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Mouse Increment. The amount of Hz that will change when moving the mousewheel is set here.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
    <property name="value">
     <number>5</number>
    </property>
   </widget>
   <widget class="QComboBox" name="AGC_select">
    <property name="geometry">
     <rect>
      <x>360</x>
      <y>550</y>
      <width>121</width>
      <height>21</height>
     </rect>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Agc, automatic gain control, will be enabled when the agc is switched on and the signal strength exceeds&lt;/p&gt;&lt;p&gt;the value in the spinbox. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
    <item>
     <property name="text">
      <string>AGC off</string>
     </property>
    </item>
    <item>
     <property name="text">
      <string>slow</string>
     </property>
    </item>
    <item>
     <property name="text">
      <string>fast</string>
     </property>
    </item>
   </widget>
   <widget class="QSpinBox" name="agc_thresholdSlider">
    <property name="geometry">
     <rect>
      <x>480</x>
      <y>550</y>
      <width>81</width>
      <height>21</height>
     </rect>
    </property>
    <property name="minimum">
     <number>-144</number>
    </property>
    <property name="maximum">
     <number>0</number>
    </property>
    <property name="value">
     <number>-100</number>
    </property>
   </widget>
   <widget class="QLabel" name="timeDisplay">
    <property name="geometry">
     <rect>
      <x>590</x>
      <y>400</y>
      <width>161</width>
      <height>21</height>
     </rect>
    </property>
    <property name="text">
     <string/>
    </property>
   </widget>
   <widget class="QLabel" name="label_2">
    <property name="geometry">
     <rect>
      <x>30</x>
      <y>0</y>
      <width>21</width>
      <height>21</height>
     </rect>
    </property>
    <property name="font">
     <font>
      <pointsize>14</pointsize>
     </font>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;SWRadio copyright:&lt;/p&gt;&lt;p&gt;Copyright of the Qt toolkit used: the Qt Company&lt;/p&gt;&lt;p&gt;Copyright of the libraries used for SDRplay, portaudio, libsndfile and libsamplerate to their developers&lt;/p&gt;&lt;p&gt;Copyright of the Reed Solomon Decoder software: Phil Karns&lt;/p&gt;&lt;p&gt;All copyrights gratefully acknowledged&lt;/p&gt;&lt;p&gt;CopyRight: 2018, Jan van Katwijk, Lazy Chair Computing.&lt;/p&gt;&lt;p&gt;sdr-j swradio (an SDR-J program) is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
    <property name="text">
     <string>©</string>
    </property>
    <property name="alignment">
     <set>Qt::AlignCenter</set>
    </property>
   </widget>
   <widget class="QPushButton" name="dumpButton">
    <property name="geometry">
     <rect>
      <x>410</x>
      <y>0</y>
      <width>111</width>
      <height>31</height>
     </rect>
    </property>
    <property name="toolTip">
     <string>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The incoming data stream can be written to a regular pcm (i.e. .wav) file. Touching this button will&lt;/p&gt;&lt;p&gt;show a menu for selecting a filename, after selecting the filename writing will start (the text on the&lt;/p&gt;&lt;p&gt;button will show &amp;quot;writing&amp;quot;). Touching the button again will close the file.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</string>
    </property>
    <property name="text">
     <string>dump</string>
    </property>
   </widget>
   <widget class="QwtPlot" name="audioSpectrum">
    <property name="geometry">
     <rect>
      <x>20</x>
      <y>290</y>
      <width>221</width>
      <height>231</height>
     </rect>
    </property>
   </widget>
   <widget class="QLabel" name="bandLabel">
    <property name="geometry">
     <rect>
      <x>470</x>
      <y>40</y>
      <width>251</width>
      <height>21</height>
     </rect>
    </property>
    <property name="font">
     <font>
      <pointsize>12</pointsize>
      <weight>75</weight>
      <italic>true</italic>
      <bold>true</bold>
     </font>
    </property>
    <property name="text">
     <string/>
    </property>
   </widget>
   <zorder>hfSpectrum</zorder>
   <zorder>hfScopeSlider</zorder>
   <zorder>lfSpectrum</zorder>
   <zorder>lfScopeSlider</zorder>
   <zorder>decoderTable</zorder>
   <zorder>freqButton</zorder>
   <zorder>bandSelector</zorder>
   <zorder>streamOutSelector</zorder>
   <zorder>freqSave</zorder>
   <zorder>middleButton</zorder>
   <zorder>mouse_Inc</zorder>
   <zorder>AGC_select</zorder>
   <zorder>agc_thresholdSlider</zorder>
   <zorder>timeDisplay</zorder>
   <zorder>label_2</zorder>
   <zorder>dumpButton</zorder>
   <zorder>frequencyDisplay</zorder>
   <zorder>audioSpectrum</zorder>
   <zorder>bandLabel</zorder>
  </widget>
  <widget class="QStatusBar" name="statusbar"/>
 </widget>
 <customwidgets>
  <customwidget>
   <class>QwtPlot</class>
   <extends>QFrame</extends>
   <header>qwt_plot.h</header>
  </customwidget>
 </customwidgets>
 <resources/>
 <connections/>
</ui>
