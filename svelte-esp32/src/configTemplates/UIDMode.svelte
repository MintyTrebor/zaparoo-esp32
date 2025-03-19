<script lang="ts">
    import { FontAwesomeIcon } from '@fortawesome/svelte-fontawesome';
    import UidExtdRec from './UIDExtdRec.svelte';
    import FileManager from './FileManager.svelte';
    import { UIDUtils } from "../backend/UIDUtils";
    import { onDestroy, onMount } from 'svelte';
    let activeLink: string = "uid"; 
    onMount(()=>{
      UIDUtils.setUIDMode(true);
    })
    onDestroy(() => {
      //UIDUtils.setUIDMode(false);
      
    });
</script>

<div class="h-100">
  <div class="text-center"> 
    <h2>UID Control Mode <FontAwesomeIcon icon="music"></FontAwesomeIcon></h2>
  </div>
  <div class="container pt-1 mt-1 h-100">
    <div class="row justify-content-center h-100">
      <div class="col-md-12 col-lg-12">
        <UidExtdRec></UidExtdRec>
      </div>
    </div>
  </div>
</div>
