<script lang="ts">
    import { FontAwesomeIcon } from '@fortawesome/svelte-fontawesome';
    import { Collapse, Toast } from 'bootstrap';
    import { EspUtils } from './backend/EspUtils';
    import { LogUtils } from './backend/LogUtils';
    import CreateMode from './configTemplates/CreateMode.svelte';
    import Esp32Config from './configTemplates/Esp32Config.svelte';
    import Esp32Defaults from './configTemplates/Esp32Defaults.svelte';
    import UidMode from './configTemplates/UIDMode.svelte';
    import WifiConfig from './configTemplates/WifiConfig.svelte';
    import ZaparooConfig from './configTemplates/ZaparooConfig.svelte';
    import logo from './lib/assets/ZapEspLogo.webp';
    import type { NavBarLink } from './types/NavBarLink';


    let activeLink: string = ""; 
    EspUtils.initWebSocket();
    let toastElement: HTMLElement;
    let toastInstance: Toast;
    let alertText = "";
    const links: NavBarLink[] = [
      { name: 'CREATE', id: 'create', icon: ['fab', 'nfc-symbol'] },
      { name: 'UID CONTROL', id: 'uid-control', icon: 'music' },
      { name: 'ZAPAROO', id: 'zaparoo', icon: 'gear' },
      { name: 'ESP32', id: 'esp32', icon: 'wrench' },
      { name: 'DEFAULTS', id: 'defaults', icon: 'sliders' },
      { name: 'Wifi', id: 'wifi', icon: 'wifi' },
    ];
    let navbarElement: HTMLDivElement;
    const setActiveLink = (linkId: string): void => {
      const collapse = Collapse.getInstance(navbarElement);
      if (collapse) {
        collapse.hide();
      }
      activeLink = linkId;
    };
    
    const showToast = () => {
      if (toastElement) {
        toastInstance = new Toast(toastElement);
        toastInstance.show();
      }
    };
    LogUtils.getNotification().subscribe(value=>{
      if(value){
        alertText = value;
        showToast();
      }
    });


</script>
<main>
  <nav class="navbar navbar-expand-md navbar-dark bg-dark shadow-sm fixed-top">
    <div class="container">
      <img alt="ZapEsp Logo" src={logo} />
      <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarNav" aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
        <span class="navbar-toggler-icon"></span>
      </button>
      <div class="collapse navbar-collapse" id="navbarNav" bind:this={navbarElement}>
        <ul class="navbar-nav ms-auto">
          {#each links as { name, id, icon }}
            <li class="nav-item">
              <a class="nav-link {activeLink === id ? 'active' : ''}" href="#" on:click={(e) => setActiveLink(id)}>
                {name} <FontAwesomeIcon icon={icon}></FontAwesomeIcon>
              </a>
            </li>
          {/each}
        </ul>
      </div>
    </div>
  </nav>
  <div class="toast-container position-fixed bottom-0 end-0 p-3" style="z-index: 11">
    <div bind:this={toastElement} id="liveToast" class="toast" role="alert" aria-live="assertive" aria-atomic="true" data-bs-delay="5000">
      <div class="toast-header">
        <strong class="me-auto">Notification</strong>
        <small>now</small>
        <button type="button" class="btn-close" data-bs-dismiss="toast" aria-label="Close"></button>
      </div>
      <div class="toast-body">
        {alertText}
      </div>
    </div>
  </div>
  <div id="mmenu_screen" class="container-fluid main_container d-flex pt-5 mt-5">
    <div class="row justify-content-center flex-fill">
        {#if activeLink === 'zaparoo'}
          <div class="col-md-8 col-lg-6">
            <ZaparooConfig></ZaparooConfig>
          </div>
        {/if}
        {#if activeLink === 'esp32'}
          <div class="col-md-8 col-lg-6">
            <Esp32Config></Esp32Config>
          </div>
        {/if}
        {#if activeLink === 'defaults'}
          <div class="col-md-8 col-lg-8">  
            <Esp32Defaults></Esp32Defaults>
          </div>
        {/if}
        {#if activeLink === 'create'}
          <div class="col-md-10 col-lg-10">  
            <CreateMode></CreateMode>
          </div>
        {/if}
        {#if activeLink === 'uid-control'}
          <div class="col-md-10 col-lg-10">  
            <UidMode></UidMode>
          </div>
        {/if}
        {#if activeLink === 'wifi'}
          <div class="col-md-8 col-lg-8">  
            <WifiConfig></WifiConfig>
          </div>
        {/if}
    </div>
  </div>
</main>