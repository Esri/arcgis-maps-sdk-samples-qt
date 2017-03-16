<h1>Local Server Services</h1>

<p>Demonstrates how to start and stop a Local Server and start and stop a Local Map, Feature, and Geoprocessing Service to that server.</p>

<p><b>Note:</b> Local Server is not supported on MacOS</p>

<p><img src="LocalServerServices.png"/></p>

<h2>How to use the sample</h2>

<p>Local Server Controls (Top Left): </p>
  <ul><li> Start Local Server -- Starts a Local Server if one is not already running.</li>
  <li> Stop Local Server --  Stops a Local Server if one is running. </li></ul>

<p>Local Services Controls (Top Right): </p>
  <ul><li> Combo Box -- Allows for the selection of a Local Map, Feature, or Geoprocessing Service.</li>
  <li> Start Service -- Starts the Service that is selected in the combo box.</li>
  <li> Stop Service --  Stops the Service that is selected in the <code>List of Running Services</code>.</li></ul>

<p>Text Area (Middle): </p>
  <ul><li> Displays the running status of the Local Server and any services that are added to that server. </li></ul>

<p>List of Running Services (Bottom): </p>
  <ul><li> Displays any services that are currently running on the server.</li> 
  <li> Clicking on the service -- Opens browser to the service. </li></ul>

<h2>How it works</h2>

<p>To start a <code>LocalServer</code> and start a <code>LocalService</code> to it:</p>

<ol>
<li>Create and run a local server.
<ul><li><code>LocalServer::instance</code> creates a local server</li>
<li><code>LocalServer::start()</code> starts the server asynchronously</li></ul></li>
<li>Wait for server to be in the  <code>LocalServerStatus::STARTED</code> state.
<ul><li><code>LocalServer::statusChanged()</code> fires whenever the running status of the local server has changed.</li></ul></li>
<li>Create and run a local service, example of running a <code>LocalMapService</code>.
<ul><li><code>new LocalMapService(Url)</code>, creates a local map service with the given url path to mpk file</li>
<li><code>Service.startAsync()</code>, starts the service asynchronously</li>
<li>service will be added to the local server automatically </li></ul></li>
</ol>

<p>To stop a <code>LocalServer</code> and stop any <code>LocalService</code>s that are added to it:</p>

<ol>
<li>Get any services that are currently running on the local server, <code>LocalServer::services()</code>.</li>
<li>Loop through all services and stop the selected service (from the dropdown) if started.
<ul><li>check service is started, <code>LocalService::status()</code> equals <code>LocalServerStatus.STARTED</code></li>
<li><code>LocalService::stop()</code>, stops the service asynchronously</li></ul></li>
<li>Wait for all services to be in the <code>LocalServerStatus.STOPPED</code> state.
<ul><li><code>LocalService::statusChanged()</code> fires whenever the running status of the local service has changed.</li></ul></li>
<li>Stop the local server, <code>LocalServer::stop()</code>.</li>
</ol>

<h2>Features</h2>

<ul>
<li>LocalFeatureService</li>
<li>LocalGeoprocessingService</li>
<li>LocalMapService</li>
<li>LocalServer</li>
<li>StatusChangedEvent</li>
<li>LocalServerStatus</li>
<li>LocalService</li>
</ul>